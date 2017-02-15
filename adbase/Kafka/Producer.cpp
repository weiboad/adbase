#include <sys/time.h>
#include <adbase/Logging.hpp>
#include <adbase/Kafka.hpp>

namespace adbase {
namespace kafka {
namespace detail {
// {{{ void deliveredCallback()

void deliveredCallback(rd_kafka_t *rk, void *payload, size_t len, rd_kafka_resp_err_t err, void *opaque, void *msg_opaque) {
	(void)rk;
	(void)payload;
	(void)len;
	(void)opaque;
	KafkaContext* data = reinterpret_cast<KafkaContext*>(msg_opaque);
	data->context->deliveredCallback(err, data->ackCode);
}

// }}}
}
// {{{ Producer::Producer()

Producer::Producer(const std::string& brokerList, int queueLen, const std::string& debug) :
	_brokerList(brokerList), 
	_queueLen(queueLen),
	_debug(debug) {
}

// }}}
// {{{ Producer::~Producer()

Producer::~Producer() {
}

// }}}
// {{{ void Producer::start()

void Producer::start() {
	_isRuning = true;
	if (!init()) {
		return;	
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

void Producer::threadFunc(void *data) {
	(void)data;
	LOG_INFO << "Start productor";
		
	while (_isRuning) {
		std::string topicName;
		adbase::Buffer message;
		int partId = 0;
		uint64_t ackCode = 0;
		if (_sendCallback == nullptr) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			continue;
		}

		bool ret = _sendCallback(topicName, &partId, message, &ackCode);
		if (!ret) {
			rd_kafka_poll(_kt, 0);
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			continue;
		}

		if (topicName.empty()) {
			if (_errorCallback) {
				_errorCallback(ackCode);							
			}	
			continue;
		}

		if (message.readableBytes() == 0) {
			continue;
		}

		if (_ktopics.find(topicName) == _ktopics.end()) {
			LOG_ERROR << "Start create topic object `" << topicName << "`";
			rd_kafka_topic_conf_t* tconf = rd_kafka_topic_conf_new();
			_ktconfs[topicName] = tconf;
			rd_kafka_topic_t* topic = rd_kafka_topic_new(_kt, topicName.c_str(), _ktconfs[topicName]);
			if (topic == nullptr) {
				LOG_ERROR << "Create kafka topic instance error, " << _error;	
				if (_errorCallback) {
					_errorCallback(ackCode);
				}
				continue;
			}
			_ktopics[topicName] = topic;
		}
		
		KafkaContext* ackContext = new KafkaContext;
		ackContext->ackCode = ackCode;
		ackContext->context = this;
		LOG_TRACE << "cor " << ackContext;

		uint32_t messageSize = static_cast<uint32_t>(message.readableBytes());
		int sendRet = rd_kafka_produce(_ktopics[topicName], partId, RD_KAFKA_MSG_F_COPY,
									   const_cast<char *>(message.peek()), messageSize,
									   nullptr, 0, ackContext);
		_kcontexts[ackCode] = ackContext;

		if (sendRet == -1) {
			LOG_ERROR << "Send message fail, topic:" << rd_kafka_topic_name(_ktopics[topicName])
					  << " partition: " << partId <<  " err: " << rd_kafka_err2str(rd_kafka_errno2err(errno)); 
			if (_errorCallback) {
				_errorCallback(ackCode);
				delete _kcontexts[ackCode];
			}
			continue;
		}
		rd_kafka_poll(_kt, 0);

		while (_isRuning && rd_kafka_outq_len(_kt) > _queueLen) {
			rd_kafka_poll(_kt, 100);
		}
	}

	LOG_INFO << "Start destroy kt.";
	rd_kafka_destroy(_kt);
	LOG_INFO << "Destroy kt.";
    int waitRunNumber = 3;
    while (waitRunNumber-- > 0 && rd_kafka_wait_destroyed(1000) == -1) {
        LOG_ERROR << "Waiting for librdkafka to decommission";
    }

	for (auto &t : _ktopics) {
		if (t.second != nullptr) {
			rd_kafka_topic_destroy(t.second);
		}	
	}
    _isClose = true;
}

// }}}
// {{{ void Producer::deliveredCallback()

void Producer::deliveredCallback(rd_kafka_resp_err_t err, uint64_t ackCode) {
	if (_kcontexts.find(ackCode) != _kcontexts.end()) {
		if (_kcontexts[ackCode] != nullptr) {
			LOG_TRACE << "dor " << _kcontexts[ackCode];
			delete _kcontexts[ackCode];
		}	
		_kcontexts[ackCode] = nullptr;
		_kcontexts.erase(ackCode);
	}

	if (err == RD_KAFKA_RESP_ERR_NO_ERROR) {
		if (_ackCallback) {
			_ackCallback(ackCode);
		}
	} else {
		if (_errorCallback) {
			_errorCallback(ackCode);
		}	
	}
}

// }}}
// {{{ bool Producer::init()

bool Producer::init() {
	_kconf = rd_kafka_conf_new();
	rd_kafka_conf_set_log_cb(_kconf, &Producer::logger);
	// 初始化配置
	initConf();

	_kt = rd_kafka_new(RD_KAFKA_PRODUCER, _kconf, const_cast<char *>(_error.c_str()), _error.size());
	if (_kt == nullptr) {
		LOG_ERROR << "Create kafka instance error, " << _error;	
		return false;
	}
	rd_kafka_set_log_level(_kt, 7);
	if (rd_kafka_brokers_add(_kt, _brokerList.c_str()) < 1) {
		LOG_ERROR << "No valid brokers specified";	
		return false;
	}

	return true;
}

// }}}
// {{{ void Producer::initConf()

void Producer::initConf() {
	rd_kafka_conf_set(_kconf, "debug", _debug.c_str(), const_cast<char *>(_error.c_str()), _error.size());
	rd_kafka_conf_set_dr_cb(_kconf, &detail::deliveredCallback);
}

// }}}
// {{{ void Producer::deleteThread()

void Producer::deleteThread(std::thread *t) {
	t->join();
	delete t;
}

// }}}
// {{{ void Producer::logger()

void Producer::logger(const rd_kafka_t *rk, int level, const char *fac, const char *buf) {
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	if (level == 7) {
		LOG_DEBUG << static_cast<int>(tv.tv_sec) << "." << static_cast<int>((tv.tv_usec / 1000))
				  << " RDKAFKA-" << level << "-" << fac << ":"
				  << rd_kafka_name(rk) << ":" << buf;
	} else if (level == 6) {
		LOG_INFO << static_cast<int>(tv.tv_sec) << "." << static_cast<int>((tv.tv_usec / 1000))
				  << " RDKAFKA-" << level << "-" << fac << ":"
				  << rd_kafka_name(rk) << ":" << buf;
	} else {
		LOG_ERROR << static_cast<int>(tv.tv_sec) << "." << static_cast<int>((tv.tv_usec / 1000))
				  << " RDKAFKA-" << level << "-" << fac << ":"
				  << rd_kafka_name(rk) << ":" << buf;
	}
}

// }}}
}
}
