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

class ConsumerBatch;
typedef std::function<bool (const std::string& topicName, int partId, uint64_t offset,
							const Buffer& message)> ConsumerMessageCallback;
typedef std::function<void (ConsumerBatch* consumer, const std::string& stats)> ConsumerBatchStatCallback;

class ConsumerBatch {
public:
	ConsumerBatch(const std::vector<std::string>& topics, const std::string& groupId, const std::string& brokerList);
	ConsumerBatch(const std::string& topic, const std::string& groupId, const std::string& brokerList);

	~ConsumerBatch();
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

	const std::string& getGroupId() {
		return _groupId;	
	}

	void setMessageHandler(const ConsumerMessageCallback& messageHandler) {
		_messageCallback = messageHandler;
	}
	const ConsumerMessageCallback& getMessageHandler() {
		return _messageCallback;
	}

    void setStatCallback(const ConsumerBatchStatCallback& statHandler) {
        _statCallback = statHandler;
    }
    void statCallback(const std::string& stat);

	void setKafkaCommitInterval(const std::string& commitInterval) {
		_kafkaCommitInterval = commitInterval;	
	}

	void setKafkaQueuedMinMessages(const std::string& queueMinMessages) {
		_kafkaQueuedMinMessages = queueMinMessages;	
	}

	void setKafkaQueuedMaxSize(const std::string& queueMaxSize) {
		_kafkaQueuedMaxSize = queueMaxSize;	
	}

	void setKafkaStatInterval(const std::string& statInterval) {
		_kafkaStatInterval = statInterval;	
	}

	void setKafkaDebug(const std::string& debug) {
		_kafkaDebug = debug;	
	}

	void setConsumeTimeout(int consumerTimeout) {
		_consumeTimeout = consumerTimeout;	
	}

	void setOffsetReset(const std::string& offsetReset) {
		_offsetReset = offsetReset;
	}

private:
    std::vector<std::string> _topics;
	std::string _groupId;
	std::string _brokerList;
	std::string _kafkaCommitInterval;
	std::string _kafkaQueuedMinMessages;
	std::string _kafkaQueuedMaxSize;
	std::string _kafkaStatInterval;
	std::string _kafkaDebug;
	std::string _offsetReset = "largest";
	int _consumeTimeout;
	std::string _offsetStoreMethod = "broker";
	bool _isRuning;
	bool _isClose = false;
	bool _isUpdate = false;
	bool _isResume = false;
	bool _isPause = false;
	typedef std::unique_ptr<std::thread, decltype(&ConsumerBatch::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool Threads;
    RdKafka::KafkaConsumer * _consumer;
	ConsumerMessageCallback _messageCallback;
    ConsumerBatchStatCallback _statCallback;
    std::unordered_map<std::string, adbase::metrics::Meters*> _meters;
	bool init();
};

} 

/*@}*/

}

#endif
