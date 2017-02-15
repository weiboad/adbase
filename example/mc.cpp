#include <adbase/Logging.hpp>
#include <adbase/Net.hpp>
#include <adbase/Mc.hpp>
#include <signal.h>

adbase::EventLoop* gEventLoop = nullptr; 
adbase::AsyncLogging* _asnclog = nullptr;

// {{{ class McProcessor

class McProcessor {
public:
	McProcessor() {}
	~McProcessor() {}

	adbase::mc::ProtocolBinaryResponseStatus set(const void* key,
			uint16_t keylen,
			const void *data,
			uint32_t datalen,
			uint32_t flags,
			uint32_t exptime,
			uint64_t cas,
			uint64_t* resultCas) {
		(void)flags;
		(void)exptime;
		(void)cas;
		(void)resultCas;
		//std::string keyData(static_cast<const char*>(key), static_cast<size_t>(keylen));
		//adbase::Buffer bufferBody;
		//bufferBody.append(static_cast<const char*>(data), static_cast<size_t>(datalen));
		//LOG_INFO << "Mc SET key:" << keyData;
		//LOG_INFO << "Mc SET data size:" << bufferBody.readableBytes();
		return adbase::mc::PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	adbase::mc::ProtocolBinaryResponseStatus get(const void* key,
			uint16_t keylen,
			adbase::Buffer *data) {
		//std::string keyData(static_cast<const char*>(key), static_cast<size_t>(keylen));
		data->append(static_cast<const char*>(key), static_cast<size_t>(keylen));
		//LOG_INFO << "Mc GET key:" << keyData;
		return adbase::mc::PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}
};
// }}}
// {{{ void asyncLogger()

void asyncLogger(const char* msg, int len) {
	if (_asnclog != nullptr) {
		_asnclog->append(msg, static_cast<int>(len));
	}
}

// }}}
// {{{ static void killSignal()

static void killSignal(const int sig) {
	(void)sig;
	if (gEventLoop != nullptr) {
		gEventLoop->stop();
		delete gEventLoop;
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
	/// 忽略Broken Pipe信号 
	signal(SIGPIPE, SIG_IGN);
	/// 处理kill信号 
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

	adbase::Logger::setOutput(std::bind(asyncLogger,
				std::placeholders::_1, std::placeholders::_2));
	// 启动异步日志落地
	std::string basename = "./logs/mc";
	_asnclog = new adbase::AsyncLogging(basename, 52428800);
	_asnclog->start();

	adbase::Logger::setTimeZone(tz);
	gEventLoop = new adbase::EventLoop();

	McProcessor mcProcessor;
	adbase::InetAddress mcAddr("0.0.0.0", 10012);
	adbase::TcpServer mcServer(gEventLoop->getBase(), mcAddr, "test");
	adbase::mc::Interface mcInterface;

	mcInterface.setSetHandler(std::bind(&McProcessor::set, &mcProcessor, std::placeholders::_1,
				std::placeholders::_2, std::placeholders::_3,
				std::placeholders::_4, std::placeholders::_5,
				std::placeholders::_6, std::placeholders::_7,
				std::placeholders::_8));
	mcInterface.setGetHandler(std::bind(&McProcessor::get, &mcProcessor, std::placeholders::_1,
				std::placeholders::_2, std::placeholders::_3));
	adbase::mc::Handler mcHandler(&mcInterface);
	mcServer.setConnectionCallback(std::bind(&adbase::mc::Handler::onConnection, &mcHandler,
				std::placeholders::_1));
	mcServer.setCloseCallback(std::bind(&adbase::mc::Handler::onClose, &mcHandler, std::placeholders::_1));
	mcServer.setMessageCallback(std::bind(&adbase::mc::Handler::onMessage, &mcHandler, std::placeholders::_1,
				std::placeholders::_2, std::placeholders::_3));
	mcServer.start(24);

	gEventLoop->start();

	return 0;
}
