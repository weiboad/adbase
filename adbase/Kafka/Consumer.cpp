#include <sys/time.h>
#include <adbase/Kafka.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Metrics.hpp>

namespace adbase {
namespace kafka {

class Consumer;
// {{{ ConsumeCb

class KConsumeCb : public RdKafka::ConsumeCb {
public:
    KConsumeCb(Consumer* consumer) : _consumer(consumer) {}

    void consume_cb (RdKafka::Message &, void *) {
    }

    ~KConsumeCb() {}
private:
    Consumer* _consumer;
};

// }}}
// {{{ EventCb

class KEventCb : public RdKafka::EventCb {
public:
    KEventCb(Consumer* consumer) : _consumer(consumer) {
        _errorEvent = adbase::metrics::Metrics::buildCounter("adbase.kafkac", "error.event"); 
        _throttledEvent = adbase::metrics::Metrics::buildCounter("adbase.kafkac", "throttled.event");
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
                _consumer->statCallback(event.str());
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
    ~KEventCb() {}
private:
    Consumer* _consumer;
    adbase::metrics::Counter* _errorEvent;
    adbase::metrics::Counter* _throttledEvent;
};

// }}}
// {{{ RebalenceCb

class KRebalanceCb : public RdKafka::RebalanceCb {
private:
    static void partPrint(const std::vector<RdKafka::TopicPartition*>&partitions){
        for (unsigned int i = 0 ; i < partitions.size() ; i++) {
            LOG_INFO << "\t" << partitions[i]->topic() << "[" << partitions[i]->partition() << "]";  
        }
    }
    Consumer* _consumer;
    adbase::metrics::Counter* _rebalanceNum;

public:
    KRebalanceCb(Consumer* consumer) : _consumer(consumer) {
        _rebalanceNum = adbase::metrics::Metrics::buildCounter("adbase.kafkac", "rebalance");
    }
    void rebalance_cb(RdKafka::KafkaConsumer *consumer,
            RdKafka::ErrorCode err,
            std::vector<RdKafka::TopicPartition*> &partitions) {
        LOG_INFO << "RebalanceCb: " << RdKafka::err2str(err) << ": ";
        partPrint(partitions);
        if (_rebalanceNum != nullptr) {
            _rebalanceNum->add(1);
        }

        if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
            consumer->assign(partitions);
        } else {
            consumer->unassign();
        }
    }
    ~KRebalanceCb() {}
};

// }}}
// {{{ Consumer::Consumer()

Consumer::Consumer(const std::unordered_map<std::string, std::string>& configs, const std::unordered_map<std::string, std::string>& tconfigs, 
		const std::vector<std::string>& topics) :
    _configs(configs),
    _tconfigs(tconfigs),
    _topics(topics),
    _consumeTimeout(10),
    _isRuning(false) {
}

// }}}
// {{{ Consumer::Consumer()

Consumer::Consumer(const std::unordered_map<std::string, std::string>& configs, const std::unordered_map<std::string, std::string>& tconfigs, const std::string& topic) :
    _configs(configs),
    _tconfigs(tconfigs),
    _consumeTimeout(10),
    _isRuning(false) {
    std::vector<std::string> topics;
    topics.push_back(topic);
    _topics = topics;
}

// }}}
// {{{ Consumer::~Consumer()

Consumer::~Consumer() {
}

// }}}
// {{{ void Consumer::pause()

void Consumer::pause() {
    _isPause = true;
}

// }}}
// {{{ void Consumer::resume()

void Consumer::resume() {
    _isResume = true;
}

// }}}
// {{{ void Consumer::stop()

void Consumer::stop() {
    _isRuning = false;
    int tryNum = 10;
    while (!_isClose && tryNum-- > 0) {
        LOG_ERROR << "Stopping consumer.";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// }}}
// {{{ void Consumer::start()

void Consumer::start() {
    if (!init()) {
        return;
    }
    _isRuning = true;
    ThreadPtr Thread(new std::thread(std::bind(&Consumer::threadFunc, this, std::placeholders::_1), nullptr), &Consumer::deleteThread);
    Threads.push_back(std::move(Thread));
}

// }}}
// {{{ void Consumer::setTopics()

void Consumer::setTopics(const std::vector<std::string> topics) {
    _topics = topics;
    _isUpdate = true;
}

// }}}
// {{{ void Consumer::threadFunc()

void Consumer::threadFunc(void *) {
    while(_isRuning) {
        RdKafka::Message *message = _consumer->consume(_consumeTimeout);
        adbase::Buffer buffer;
        std::string topicName = message->topic_name();
        int32_t partId = message->partition();
        adbase::metrics::Meters* meter;
        std::string metricName = "consumer." + topicName + "." + std::to_string(partId) + "." + _configs["group.id"]; 
        std::unordered_map<std::string, std::string> tags;
        tags["topic_name"] = topicName;
        tags["part_id"] = std::to_string(partId);
        tags["group_id"] = _configs["group.id"];
        switch (message->err()) {
            case RdKafka::ERR__TIMED_OUT:
                break;

            case RdKafka::ERR_NO_ERROR:
                if (_meters.find(metricName) == _meters.end()) {
                    meter = adbase::metrics::Metrics::buildMetersWithTag("adbase.kafkac", "kafkac", tags);
                    _meters[metricName] = meter;
                } else {
                    meter = _meters[metricName]; 
                }
                if (meter != nullptr) {
                    meter->mark();
                }
                /* Real message */
                LOG_TRACE << "Read msg at offset " << message->offset() << " topic:" << topicName << " part:" << partId;
                buffer.append(static_cast<const char *>(message->payload()), static_cast<int>(message->len()));
                _messageCallback(topicName, partId, message->offset(), buffer);
                break;

            case RdKafka::ERR__PARTITION_EOF:
                LOG_DEBUG << "Consume failed: " << message->errstr();
                break;
            case RdKafka::ERR__UNKNOWN_TOPIC:
            case RdKafka::ERR__UNKNOWN_PARTITION:
                LOG_ERROR << "Consume failed: " << message->errstr();
                break;

            default:
                /* Errors */
                LOG_ERROR << "Consume failed: " << message->errstr();
        }
        delete message;

        if (_isUpdate) {
            _consumer->unsubscribe();
            _consumer->unassign();
            _consumer->subscribe(_topics);
            _isUpdate = false;
            LOG_INFO << "Update topics is success.";
        }

        if (_isPause) {
            _consumer->unsubscribe();
            _consumer->unassign();
            LOG_INFO << "pause consumer success.";
            _isPause = false;
        }
        if (_isResume) {
            _consumer->unsubscribe();
            _consumer->unassign();
            _consumer->subscribe(_topics);
            LOG_INFO << "resume consumer success.";
            _isResume = false;
        }
    }
    _consumer->close();
    delete _consumer;
    RdKafka::wait_destroyed(5000);
    _isClose = true;
}

// }}}
// {{{ bool Consumer::init()

bool Consumer::init() {
    std::string errstr;
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    // 初始化配置
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

	for (auto & config : _configs) {
		conf->set(config.first, config.second, errstr);
	}
    KRebalanceCb* rebalanceCb = new KRebalanceCb(this);
    if (RdKafka::Conf::CONF_OK != conf->set("rebalance_cb", rebalanceCb, errstr)) {
        LOG_ERROR << errstr; 
    }
    KEventCb* eventCb = new KEventCb(this);
    if (RdKafka::Conf::CONF_OK != conf->set("event_cb", eventCb, errstr)) {
        LOG_ERROR << errstr; 
    }
    //KConsumeCb* consumeCb = new KConsumeCb(this);
    //if (RdKafka::Conf::CONF_OK != conf->set("consume_cb", consumeCb, errstr)) {
    //    LOG_ERROR << errstr; 
    //}
	for (auto & config : _tconfigs) {
		tconf->set(config.first, config.second, errstr);
	}
	
    std::list<std::string> *dump;
    dump = conf->dump();
    for (auto it = dump->begin(); it != dump->end();)   {
        std::string key = *it;
        it++;
        std::string value = *it;
        it++;
        LOG_INFO << "Global config:" << key << "=" << value;
    }
    dump = tconf->dump();
    for (auto it = dump->begin(); it != dump->end();)   {
        std::string key = *it;
        it++;
        std::string value = *it;
        it++;
        LOG_INFO << "Topic config:" << key << "=" << value;
    }
    conf->set("default_topic_conf", tconf, errstr);
    _consumer = RdKafka::KafkaConsumer::create(conf, errstr);
    delete tconf;
    delete conf;

    RdKafka::ErrorCode err = _consumer->subscribe(_topics);
    if (err) {
        LOG_SYSFATAL << "Failed to subscribe to " << _topics.size() << " topics: "
            << RdKafka::err2str(err);
    }
    LOG_INFO << "% Created consumer " << _consumer->name() << " rdkafka version:" << RdKafka::version_str();

    return true;
}

// }}}
// {{{ void Consumer::deleteThread()

void Consumer::deleteThread(std::thread *t) {
    t->join();
	LOG_ERROR << "Consumer main thread stop.";
	delete t;
}

// }}}
// {{{ void Consumer::statCallback()

void Consumer::statCallback(const std::string& stat) {
    if (_statCallback) {
        _statCallback(this, stat);
    }
}

// }}}
}
}
