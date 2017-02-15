#include <adbase/Kafka.hpp>
#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>
#include <signal.h>

adbase::EventLoop* gEventLoop = nullptr; 
adbase::kafka::Consumer* gConsumer = nullptr; 

// {{{ class PullMessage

class PullMessage {
public:
	PullMessage() {}
	~PullMessage() {}

	bool pull(const std::string& topicName, int partId, uint64_t offset, const adbase::Buffer& message) {	
		(void)topicName;
		LOG_ERROR << "Size:" << message.readableBytes();
		LOG_ERROR << partId << "-Offset:" << offset;
		//LOG_INFO << "Topic:" << topicName << " partId:" << partId << " Msg:" << message;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		return true;
	}

	void stat(adbase::kafka::Consumer* consumer, const std::string& stats) {
		(void)consumer;
		LOG_INFO << "Stats:" << stats.substr(0, 1024);
	}
};

// }}}
// {{{ static void killSignal()

static void killSignal(const int sig) {
	(void)sig;
	if (gEventLoop != nullptr) {
		gEventLoop->stop();
		delete gEventLoop;
	}
	if (gConsumer != nullptr) {
		gConsumer->stop();
		delete gConsumer;
	}
	exit(0);
}

// }}}
// {{{ static void reloadConf()

static void reloadConf(const int sig) {
	(void)sig;
}

// }}}
// {{{ static void registerSignal()

static void registerSignal() {
	/* 忽略Broken Pipe信号 */
	signal(SIGPIPE, SIG_IGN);
	/* 处理kill信号 */
	signal(SIGINT,  killSignal);
	signal(SIGKILL, killSignal);
	signal(SIGQUIT, killSignal);
	signal(SIGTERM, killSignal);
	signal(SIGHUP,  killSignal);
	signal(SIGSEGV, killSignal);
	signal(SIGUSR1, reloadConf);
}

// }}}

int main(void) {
	registerSignal();
	adbase::TimeZone tz(8*3600, "CST");
	adbase::Logger::setTimeZone(tz);
	gEventLoop = new adbase::EventLoop();

	PullMessage pullMessage;
	gConsumer = new adbase::kafka::Consumer("test", "testgroupid", "10.13.4.159:9192");
	gConsumer->setMessageHandler(std::bind(&PullMessage::pull, &pullMessage, std::placeholders::_1,
			   						       std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	gConsumer->setStatCallback(std::bind(&PullMessage::stat, &pullMessage, std::placeholders::_1,
			   						     std::placeholders::_2));
	gConsumer->setKafkaStatInterval("1000");
	gConsumer->setKafkaDebug("msg");
	gConsumer->setIsNewConsumer(true);
	gConsumer->setOffsetStoreMethod("broker");
	gConsumer->start();

	gEventLoop->start();

	return 0;
}
