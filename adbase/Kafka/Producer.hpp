#if !defined ADBASE_KAFKA_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_KAFKA_PRODUCTOR_HPP_
#define ADBASE_KAFKA_PRODUCTOR_HPP_

#include <thread>
#include <vector>
#include <unordered_map>
#include <librdkafka/rdkafka.h>

namespace adbase {

/**
 * @addtogroup kafka adbase::Kafka
 */
/*@{*/

class Buffer;
namespace kafka {

namespace detail {
void deliveredCallback(rd_kafka_t *rk, void *payload, size_t len, rd_kafka_resp_err_t err, void *opaque, void *msg_opaque);
}
class Producer;
typedef std::function<bool (std::string& topicName, int* partId,
							Buffer& message, uint64_t* ackCode)> MessageSendCallback;
typedef std::function<void (uint64_t ackCode)> MessageAckCallback;
typedef std::function<void (uint64_t ackCode)> MessageErrorCallback;

typedef struct KafkaContext {
	uint64_t ackCode;
	Producer* context;
} KafkaContext;

class Producer {
public:
	Producer(const std::string& brokerList, int queueLen, const std::string& debug);
	~Producer();
	void start();
	void stop();
    void threadFunc(void *data);
	void deliveredCallback(rd_kafka_resp_err_t err, uint64_t ackCode);
	static void deleteThread(std::thread *t);
	static void logger(const rd_kafka_t *rk, int level, const char *fac, const char *buf);

	void setSendHandler(const MessageSendCallback& sendHandler) {
		_sendCallback = sendHandler;
	}

	void setErrorHandler(const MessageErrorCallback& errorHandler) {
		_errorCallback = errorHandler;
	}

	void setAckHandler(const MessageAckCallback& ackHandler) {
		_ackCallback = ackHandler;
	}

private:
	typedef std::unique_ptr<std::thread, decltype(&Producer::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool Threads;
	typedef std::shared_ptr<rd_kafka_conf_t> Kconf;
	rd_kafka_t* _kt;
	rd_kafka_conf_t* _kconf;
	std::unordered_map<std::string, rd_kafka_topic_t*> _ktopics;
	std::unordered_map<std::string, rd_kafka_topic_conf_t*> _ktconfs;
	std::unordered_map<uint64_t, KafkaContext*> _kcontexts;
	std::string _error;
	bool _isRuning;
	bool _isClose = false;
	std::string _brokerList;
	int _queueLen;
	std::string _debug;
	void initConf();
	bool init();
	MessageAckCallback _ackCallback;
	MessageSendCallback _sendCallback;
	MessageErrorCallback _errorCallback;
};

}

/*@}*/

}

#endif
