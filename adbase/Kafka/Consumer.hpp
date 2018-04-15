#if !defined ADBASE_KAFKA_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_KAFKA_CONSUMER_BATCH_HPP_
#define ADBASE_KAFKA_CONSUMER_BATCH_HPP_

#include <thread>
#include <vector>
#include <adbase/Metrics.hpp>
#include <librdkafka/rdkafkacpp.h>

namespace adbase {

/**
 * @addtogroup kafka adbase::Kafka
 */
/*@{*/

class Buffer;
namespace kafka {

class Consumer;
typedef std::function<bool (const std::string& topicName, int partId, uint64_t offset,
							const Buffer& message)> ConsumerMessageCallback;
typedef std::function<void (Consumer* consumer, const std::string& stats)> ConsumerStatCallback;

class Consumer {
public:
	Consumer(const std::unordered_map<std::string, std::string>& config, const std::unordered_map<std::string, std::string>& tconfig, 
			const std::vector<std::string>& topics);
	Consumer(const std::unordered_map<std::string, std::string>& config, const std::unordered_map<std::string, std::string>& tconfig, const std::string& topic);

	~Consumer();
	void start();
	void stop();
	void pause();
	void resume();
    void threadFunc(void *data);
	static void deleteThread(std::thread *t);

    void setTopics(const std::vector<std::string> topics);

	const std::vector<std::string>& getTopics() {
		return _topics;	
	}

	void setMessageHandler(const ConsumerMessageCallback& messageHandler) {
		_messageCallback = messageHandler;
	}
	const ConsumerMessageCallback& getMessageHandler() {
		return _messageCallback;
	}

    void setStatCallback(const ConsumerStatCallback& statHandler) {
        _statCallback = statHandler;
    }
    void statCallback(const std::string& stat);

	void setConsumeTimeout(int consumerTimeout) {
		_consumeTimeout = consumerTimeout;	
	}

private:
	std::unordered_map<std::string, std::string> _configs;
	std::unordered_map<std::string, std::string> _tconfigs;
    std::vector<std::string> _topics;
	std::string _kafkaCommitInterval;
	int _consumeTimeout;
	bool _isRuning;
	bool _isClose = false;
	bool _isUpdate = false;
	bool _isResume = false;
	bool _isPause = false;
	typedef std::unique_ptr<std::thread, decltype(&Consumer::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool Threads;
    RdKafka::KafkaConsumer * _consumer;
	ConsumerMessageCallback _messageCallback;
    ConsumerStatCallback _statCallback;
    std::unordered_map<std::string, adbase::metrics::Meters*> _meters;
	bool init();
};

} 

/*@}*/

}

#endif
