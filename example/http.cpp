#include <adbase/Http.hpp>
#include <adbase/Logging.hpp>
#include <signal.h>

adbase::http::Server* ghttp = nullptr;
adbase::AsyncLogging* _asnclog = nullptr;

// {{{ void asyncLogger()

void asyncLogger(const char* msg, int len) {
	if (_asnclog != nullptr) {
		_asnclog->append(msg, static_cast<int>(len));
	}
}

// }}}
// {{{ void request()

void request(adbase::http::Request* request, adbase::http::Response* response, void* data) {
	(void)request;
	if (data != nullptr) {
		std::string* context = reinterpret_cast<std::string*>(data);	
		LOG_INFO << "Context:" << *context;
	} else {
		(void)data;
	}
	response->sendReply("OK", 200, "OK");
}

// }}}
// {{{ void echo()

void echo(adbase::http::Request* request, adbase::http::Response* response, void* data) {
	(void)data;
	std::string post = request->getPostData();
	response->sendReply("OK", 200, post);
}

// }}}
// {{{ static void killSignal()

static void killSignal(const int sig) {
	(void)sig;
	if (ghttp != nullptr) {
		ghttp->stop();
		delete ghttp;
		ghttp = nullptr;
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
	adbase::http::Config config("0.0.0.0", 10010, 3);
	config.setTimeZone(tz);
	config.setServerName("test");
	config.setLogDir("logs");

	adbase::Logger::setOutput(std::bind(asyncLogger,
				std::placeholders::_1, std::placeholders::_2));
	// 启动异步日志落地
	std::string basename = "./logs/http";
	_asnclog = new adbase::AsyncLogging(basename, 52428800);
	_asnclog->start();

	adbase::http::Server* http = new adbase::http::Server(config);		
	ghttp = http;
	std::string test = "test string";
	http->registerLocation("/debug", std::bind(request, std::placeholders::_1,
							std::placeholders::_2, std::placeholders::_3), &test);
	http->registerLocation("/echo", std::bind(echo, std::placeholders::_1,
	 				std::placeholders::_2, std::placeholders::_3), nullptr);
	http->start(24);

	// do something
	
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));	
	}
	LOG_INFO << "exit.";
	return 0;
}
