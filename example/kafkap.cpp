#include <adbase/Kafka.hpp>
#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>
#include <signal.h>

adbase::EventLoop* gEventLoop = nullptr; 
adbase::kafka::Producer* gProducer = nullptr; 

// {{{ class SendMessage

class SendMessage {
public:
	SendMessage() :
		_index(0) {
	}
	~SendMessage() {}

	// 如果有数据要发送 返回 true, 否则返回false 
	bool send(std::string& topicName, int* partId, adbase::Buffer& message, uint64_t* ackCode) {	
		topicName = "test";
		message.append("TEST" + std::to_string(_index));

		*partId = rand() % 5;
		*ackCode = _index;
		if (_index == 10) {
			return false;
		}
		_index++;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		LOG_INFO << "Send message";
		return true;
	}

	void ackCallback(uint64_t ackCode) {
		LOG_INFO << "Ack send ackCode: " << ackCode;
	}

	void errorCallback(uint64_t ackCode) {	
		LOG_ERROR << "Error send ackCode: " << ackCode;
	}

private:
	uint64_t _index;
};

// }}}
// {{{ static void killSignal()

static void killSignal(const int sig) {
	(void)sig;
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
	gProducer = new adbase::kafka::Producer("10.13.4.161:9192", 1000, "none");
	gProducer->setSendHandler(std::bind(&SendMessage::send, &sendMessage, std::placeholders::_1,
									   std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	gProducer->setAckHandler(std::bind(&SendMessage::ackCallback, &sendMessage, std::placeholders::_1));
	gProducer->setErrorHandler(std::bind(&SendMessage::errorCallback, &sendMessage, std::placeholders::_1));
	gProducer->start();

	gEventLoop->start();

	return 0;
}
