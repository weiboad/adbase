#include <adbase/Kafka.hpp>
#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>
#include <signal.h>

adbase::EventLoop* gEventLoop = nullptr; 
adbase::kafka::Producer* gProducer = nullptr; 

// {{{ class SendMessage

class SendMessage {
public:
	SendMessage() {}
	~SendMessage() {}

	// 如果有数据要发送 返回 true, 否则返回false 
	bool send(std::string& topicName, int* partId, adbase::Buffer& message) {	
		topicName = "test";
		message.append("TEST" + std::to_string(_index));

		*partId = 100;
		if (_index == 10) {
			return false;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
		LOG_INFO << "Send message";
		return true;
	}

	void errorCallback(const std::string& topicName, int partId, const adbase::Buffer& message, const std::string& error) {	
        std::string messageStr = std::string(message.peek(), message.readableBytes());
		LOG_ERROR << "Error send: " << error << " part:" << partId << " topicName:" << topicName << " message:" << messageStr;
	}

private:
	uint64_t _index;
};

// }}}
// {{{ static void killSignal()

static void killSignal(const int sig) {
	(void)sig;
    LOG_ERROR << "dedede " << sig;
	if (gEventLoop != nullptr) {
		gEventLoop->stop();
		delete gEventLoop;
	}
	if (gProducer != nullptr) {
		gProducer->stop();
		delete gProducer;
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

	SendMessage sendMessage;
	gProducer = new adbase::kafka::Producer("10.13.4.162:9192", 1, "all");
	gProducer->setSendHandler(std::bind(&SendMessage::send, &sendMessage, std::placeholders::_1,
									   std::placeholders::_2, std::placeholders::_3));
	gProducer->setErrorHandler(std::bind(&SendMessage::errorCallback, &sendMessage, std::placeholders::_1,
									   std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    std::unordered_map<std::string, std::vector<uint32_t>> topics = gProducer->getTopics();
    for(auto &t : topics) {
        LOG_ERROR << t.first << ":" << t.second.size();
    }
	gProducer->start();

	gEventLoop->start();

	return 0;
}
