#if !defined ADBASE_KAFKA_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_KAFKA_CONSUMER_HPP_
#define ADBASE_KAFKA_CONSUMER_HPP_

#include <thread>
#include <vector>
#include <librdkafka/rdkafka.h>

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
	Consumer(const std::string& topicName, const std::string& groupId, const std::string& brokerList);
	~Consumer();
	void start();
	void stop();
    void threadFunc(void *data);
	static void deleteThread(std::thread *t);
	static void logger(const rd_kafka_t *rk, int level, const char *fac, const char *buf);
	static void rebalanceCallback(rd_kafka_t *rk, rd_kafka_resp_err_t err,
								  rd_kafka_topic_partition_list_t *partitions,
								  void *opaque);
	static int statsCallback(rd_kafka_t *rk, char *json, size_t jsonLen, void *opaque);

	const std::string& getTopicName() {
		return _topic;	
	}

	const std::string& getGroupId() {
		return _groupId;	
	}

	void setMessageHandler(const ConsumerMessageCallback& messageHandler) {
		_messageCallback = messageHandler;
	}

	void setStatCallback(const ConsumerStatCallback& statHandler) {
		_statCallback = statHandler;
	}
	void statCallback(Consumer* consumer, const std::string& stat);

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

	void setOffsetStorePath(const std::string& path) {
		_offsetStorePath = path;
	}

	void setOffsetStoreMethod(const std::string& method) {
		_offsetStoreMethod = method;
	}

	void setOffsetReset(const std::string& offsetReset) {
		_offsetReset = offsetReset;
	}

	void setIsNewConsumer(bool isNewConsumer) {
		_isNewConsumer = isNewConsumer;
	}

private:
	std::string _topic;
	std::string _groupId;
	std::string _brokerList;
	std::string _kafkaCommitInterval;
	std::string _kafkaQueuedMinMessages;
	std::string _kafkaQueuedMaxSize;
	std::string _kafkaStatInterval;
	std::string _kafkaDebug;
	std::string _offsetReset = "largest";
	int _consumeTimeout;
	std::string _offsetStorePath = "./";
	bool _isNewConsumer = false;
	std::string _offsetStoreMethod = "file";
	bool _isRuning;
	bool _isClose = false;
	typedef std::unique_ptr<std::thread, decltype(&Consumer::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool Threads;
	typedef std::shared_ptr<rd_kafka_conf_t> Kconf;
	typedef std::shared_ptr<rd_kafka_topic_conf_t> Ktconf;
	typedef std::shared_ptr<rd_kafka_t> Kt;
	typedef std::shared_ptr<rd_kafka_topic_t> Ktopic;
	typedef std::shared_ptr<rd_kafka_queue_t> Kqueue;
	rd_kafka_t* _kt;
	rd_kafka_conf_t* _kconf;
	rd_kafka_topic_conf_t* _ktconf;
	rd_kafka_topic_partition_list_t* _topics;
	Ktopic _ktopic;
	Kqueue _kqueue;
	std::string _error;
	ConsumerStatCallback _statCallback;
	ConsumerMessageCallback _messageCallback;
	bool _isPauseConsumer = false;
	void initConf();
	bool init();
	int getTopicPartitionNum();
};

} 

/*@}*/

}

#endif
