#include <sys/time.h>
#include <adbase/Logging.hpp>
#include <adbase/Metrics.hpp>
#include <adbase/Kafka.hpp>

namespace adbase {
namespace kafka {
class Producer;
// {{{ KDeliveredCbProducer

class KDeliveredCbProducer : public RdKafka::DeliveryReportCb {
public:
    KDeliveredCbProducer(Producer* producer) : _producer(producer) {
    }
    void dr_cb (RdKafka::Message &message) {
        std::string topicName = message.topic_name();
        std::unordered_map<std::string, std::string> tags;
        tags["topic_name"] = topicName;
        if (topicName.empty()) {
            LOG_ERROR << " topic name is empty";
            return;
        }
        if (_sendMeter.find(topicName) == _sendMeter.end()) {
            _sendMeter[topicName] = adbase::metrics::Metrics::buildMetersWithTag("adbase.kafkap", "send.count", tags); 
        }
        if (_sendMeter[topicName] != nullptr) {
            _sendMeter[topicName]->mark();
        }
        if (message.err() == RdKafka::ERR_NO_ERROR) {
            LOG_DEBUG << "Message delivery for (" << message.len() << " bytes): " << message.errstr();
        } else {
            adbase::Buffer buffer;
            buffer.append(static_cast<const void*>(message.payload()), message.len());
            _producer->errorCallback(topicName, message.partition(), buffer, message.errstr());
            if (_errorMeter.find(topicName) == _errorMeter.end()) {
                _errorMeter[topicName] = adbase::metrics::Metrics::buildMetersWithTag("adbase.kafkap", "send.error", tags); 
            }
            if (_errorMeter[topicName] != nullptr) {
                _errorMeter[topicName]->mark();
            }
        }
    }
    ~KDeliveredCbProducer() {}
private:
    Producer* _producer;
    std::unordered_map<std::string, adbase::metrics::Meters*> _sendMeter;
    std::unordered_map<std::string, adbase::metrics::Meters*> _errorMeter;
};

// }}}
// {{{ KEventCbProducer

class KEventCbProducer : public RdKafka::EventCb {
public:
    KEventCbProducer(Producer* producer) : _producer(producer) {
        _errorEvent = adbase::metrics::Metrics::buildCounter("adbase.kafkap", "error.event"); 
        _throttledEvent = adbase::metrics::Metrics::buildCounter("adbase.kafkap", "throttled.event");
    }
    void event_cb (RdKafka::Event &event) {
        switch (event.type())
        {
            case RdKafka::Event::EVENT_ERROR:
                LOG_ERROR << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                    event.str();
                if (_errorEvent != nullptr) {
                    _errorEvent->add(1);
                }
                break;

            case RdKafka::Event::EVENT_STATS:
                _producer->statCallback(event.str());
                break;

            case RdKafka::Event::EVENT_LOG:
                if (event.severity() == RdKafka::Event::EVENT_SEVERITY_DEBUG) {
                    LOG_DEBUG << "LOG-" << event.severity() << "-" << event.fac() << ":" << event.str();
                } else if (event.severity() == RdKafka::Event::EVENT_SEVERITY_INFO) {
                    LOG_INFO << "LOG-" << event.severity() << "-" << event.fac() << ":" << event.str();
                } else {
                  LOG_ERROR << "LOG-" << event.severity() << "-" << event.fac() << ":" << event.str();
                }
                break;

            case RdKafka::Event::EVENT_THROTTLE:
                LOG_INFO << "THROTTLED: " << event.throttle_time() << "ms by " <<
                    event.broker_name() << " id " << static_cast<int>(event.broker_id());
                if (_throttledEvent != nullptr) {
                    _throttledEvent->add(1);
                }
                break;

            default:
                LOG_ERROR << "EVENT " << event.type() <<
                    " (" << RdKafka::err2str(event.err()) << "): " << event.str();
                break;
        }
    }
    ~KEventCbProducer() {}
private:
    Producer* _producer;
    adbase::metrics::Counter* _errorEvent;
    adbase::metrics::Counter* _throttledEvent;
};

// }}}
// {{{ Producer::Producer()

Producer::Producer(const std::unordered_map<std::string, std::string>& configs, int queueLen) :
    _configs(configs),
	_queueLen(queueLen) {
}

// }}}
// {{{ Producer::~Producer()

Producer::~Producer() {
}

// }}}
// {{{ void Producer::start()

void Producer::start() {
	_isRuning = true;
    if (_producer == nullptr) {
        if (!init()) {
            return;
        }
    }
	ThreadPtr Thread(new std::thread(std::bind(&Producer::threadFunc, this, std::placeholders::_1), nullptr), &Producer::deleteThread);
	Threads.push_back(std::move(Thread));
}

// }}}
// {{{ void Producer::stop()

void Producer::stop() {
	_isRuning = false;
	int tryNum = 10;
	while (!_isClose && tryNum-- > 0) {
		LOG_ERROR << "Stopping productor.";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	LOG_INFO << "Stop kafka productor success";
}

// }}}
// {{{ void Producer::threadFunc()

void Producer::threadFunc(void *) {
	LOG_INFO << "Start productor";
    _isClose = false;
	while (_isRuning) {
		std::string topicName;
		adbase::Buffer message;
		int partId = 0;
		if (_sendCallback == nullptr) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		bool ret = _sendCallback(topicName, &partId, message);
		if (!ret) {
            _producer->poll(0);
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			continue;
		}

		if (topicName.empty()) {
			if (_errorCallback) {
                std::string error = "Topic name is empty";
				_errorCallback(topicName, partId, message, error);		
			}
			continue;
		}

		if (message.readableBytes() == 0) {
			continue;
		}

		if (_ktopics.find(topicName) == _ktopics.end()) {
			LOG_INFO << "Start create topic object `" << topicName << "`";
            std::string errstr;
            RdKafka::Topic *topic = RdKafka::Topic::create(_producer, topicName, _tconf, errstr);
            if (topic == nullptr) {
				LOG_ERROR << "Create kafka topic instance error, " << errstr;	
                if (_errorCallback) {
				    _errorCallback(topicName, partId, message, errstr);		
                }
                continue;
            }
			_ktopics[topicName] = topic;
		}

        if (partId == -1) {
            partId = RdKafka::Topic::PARTITION_UA;
        }
		
		uint32_t messageSize = static_cast<uint32_t>(message.readableBytes());
        RdKafka::ErrorCode resp = _producer->produce(_ktopics[topicName], partId,
                                RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                                const_cast<char *>(message.peek()), messageSize, nullptr, nullptr);
		if (resp != RdKafka::ERR_NO_ERROR) {
			LOG_ERROR << "Send message fail, topic:" << topicName
					  << " partition: " << partId <<  " err: " << RdKafka::err2str(resp);
			if (_errorCallback) {
				_errorCallback(topicName, partId, message, RdKafka::err2str(resp));
			}
			continue;
		}
        _producer->poll(0);

		while (_isRuning && _producer->outq_len() > _queueLen) {
            _producer->poll(100);
		}
	}

    RdKafka::ErrorCode errorCode = _producer->flush(3000);
    if (errorCode == RdKafka::ERR__TIMED_OUT) {
        LOG_ERROR << "Flush message timeout, message size:" << _producer->outq_len();  
    }

    for (auto &t : _ktopics) {
        if (t.second != nullptr) {
            delete t.second;
            t.second = nullptr;
        }
    }
    delete _producer;
    delete _tconf;
    delete _conf;
    _isClose = true;
}

// }}}
// {{{ bool Producer::init()

bool Producer::init() {
    std::string errstr;
    _conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    // 初始化配置
    _tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    
    KEventCbProducer* eventCb = new KEventCbProducer(this);
    if (RdKafka::Conf::CONF_OK != _conf->set("event_cb", eventCb, errstr)) {
        LOG_ERROR << errstr; 
        return false;
    }
    _drCb = new KDeliveredCbProducer(this);
    if (RdKafka::Conf::CONF_OK != _conf->set("dr_cb", _drCb, errstr)) {
        LOG_ERROR << errstr; 
        return false;
    }

    for (auto config : _configs) {
        _conf->set(config.first, config.second, errstr);
    }

    std::list<std::string> *dump;
    dump = _conf->dump();
    for (auto it = dump->begin(); it != dump->end();)   {
        std::string key = *it;
        it++;
        std::string value = *it;
        it++;
        LOG_INFO << "Global config:" << key << "=" << value;
    }
    dump = _tconf->dump();
    for (auto it = dump->begin(); it != dump->end();)   {
        std::string key = *it;
        it++;
        std::string value = *it;
        it++;
        LOG_INFO << "Topic config:" << key << "=" << value;
    }
    _producer = RdKafka::Producer::create(_conf, errstr);
    if (_producer == nullptr) {
        LOG_SYSFATAL << "Failed to create producer:" << errstr;
    }
    LOG_INFO << "Created producer " << _producer->name();
    return true;
}

// }}}
// {{{ void Producer::deleteThread()

void Producer::deleteThread(std::thread *t) {
	t->join();
	delete t;
}

// }}}
    // {{{ std::unordered_map<std::string, std::vector<uint32_t>> getTopics()
    
    std::unordered_map<std::string, std::vector<uint32_t>> Producer::getTopics(uint32_t timeout) {
        std::unordered_map<std::string, std::vector<uint32_t>> result;
        if (_producer == nullptr) {
            if (!init()) {
                return result;
            }
        }
        
        RdKafka::Metadata* metadata = nullptr;
        RdKafka::ErrorCode code = _producer->metadata(true, nullptr, &metadata, timeout);
        if (code != RdKafka::ERR_NO_ERROR) {
            LOG_ERROR << "Get kafka metadata fail, err:" << RdKafka::err2str(code);
            return result;
        }

        const RdKafka::Metadata::TopicMetadataVector* topicsVec = metadata->topics();
        for (auto it = topicsVec->begin(); it != topicsVec->end(); it++) {
            const RdKafka::TopicMetadata::PartitionMetadataVector* partVec = (*it)->partitions();
            std::vector<uint32_t> ids;
            for (auto pit = partVec->begin(); pit != partVec->end(); pit++) {
                ids.push_back((*pit)->id());
            }
            
            result[(*it)->topic()] = ids;
        }

        if (metadata != nullptr) {
            delete metadata;
        }

        return result;
    }

    // }}}
}
}
