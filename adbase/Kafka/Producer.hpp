#if !defined ADBASE_KAFKA_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_KAFKA_PRODUCTOR_HPP_
#define ADBASE_KAFKA_PRODUCTOR_HPP_

#include <thread>
#include <vector>
#include <unordered_map>
#include <librdkafka/rdkafkacpp.h>

namespace adbase {

/**
 * @addtogroup kafka adbase::Kafka
 */
/*@{*/

class Buffer;
namespace kafka {

class Producer;
typedef std::function<bool (std::string& topicName, int* partId,
							Buffer& message)> MessageSendCallback;
typedef std::function<void (const std::string& topicName, int partId, const Buffer& message, const std::string& error)> MessageErrorCallback;
typedef std::function<void (const std::string& stat)> StatCallback;

class KDeliveredCbProducer;
class Producer {
public:
	Producer(const std::unordered_map<std::string, std::string>& config, int queueLen);
	~Producer();
	void start();
	void stop();
    void threadFunc(void *data);
	static void deleteThread(std::thread *t);

	void setSendHandler(const MessageSendCallback& sendHandler) {
		_sendCallback = sendHandler;
	}

	void setErrorHandler(const MessageErrorCallback& errorHandler) {
		_errorCallback = errorHandler;
	}

	void setStatHandler(const StatCallback& statHandler) {
         _statCallback = statHandler;
	}

    void statCallback(std::string stat) {
        if (_statCallback) {
            _statCallback(stat);
        }
    }

    void errorCallback(const std::string& topicName, int partId, const Buffer& message, const std::string& error) {
        if (_errorCallback) {
            _errorCallback(topicName, partId, message, error);
        }
    }

    std::unordered_map<std::string, std::vector<uint32_t>> getTopics(uint32_t timeout = 3000);

private:
	typedef std::unique_ptr<std::thread, decltype(&Producer::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool Threads;

	std::unordered_map<std::string, RdKafka::Topic*> _ktopics;
    RdKafka::Producer* _producer = nullptr;

	std::unordered_map<std::string, std::string> _configs;

	bool _isRuning;
	bool _isClose = true;

	int _queueLen;

    RdKafka::Conf* _conf;
    RdKafka::Conf* _tconf;
    KDeliveredCbProducer* _drCb;

	bool init();
	MessageSendCallback _sendCallback;
	MessageErrorCallback _errorCallback;
    StatCallback _statCallback;
};

}

/*@}*/

}

#endif
