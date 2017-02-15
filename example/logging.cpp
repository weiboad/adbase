#include <adbase/Logging.hpp>
#include <signal.h>

adbase::AsyncLogging* glogger = nullptr;
void asyncLogger(const char* msg, size_t len) {
	if (glogger != nullptr) {
		glogger->append(msg, len);
	}
}
// {{{ static void killSignalMaster()

static void killSignalMaster(const int sig) {
	(void)(sig);

	LOG_ERROR << sig;
	if (glogger != nullptr) {
		LOG_INFO << "Stop...";
		delete glogger;
		glogger = nullptr;
	}

	/* 给进程组发送SIGTERM信号，结束子进程 */
	kill(0, SIGTERM);

	exit(0);
}

// }}}

int main() {
	/* 处理kill信号 */
	signal (SIGINT, killSignalMaster);
	signal (SIGKILL, killSignalMaster);
	signal (SIGQUIT, killSignalMaster);
	signal (SIGTERM, killSignalMaster);
	signal (SIGHUP, killSignalMaster);
	signal (SIGSEGV, killSignalMaster);

	// 设置日志级别
	adbase::Logger::setLogLevel(adbase::Logger::TRACE);
	LOG_TRACE << "trace test";
	LOG_DEBUG << "debug test";
	LOG_INFO  << "info test";
	LOG_ERROR << 45;
	// 格式化的方式写入
	LOG_INFO  << adbase::Fmt("%d test", 45);
	adbase::AsyncLogging*logger = new adbase::AsyncLogging("./test", 5 * 1024 * 1024);
	glogger = logger;
	adbase::Logger::setOutput(std::bind(&asyncLogger, std::placeholders::_1,  std::placeholders::_2));
	glogger->start();

	LOG_TRACE << "trace test";
	LOG_DEBUG << "debug test";
	LOG_INFO  << "info test";
	LOG_ERROR << 45;
	// 格式化的方式写入
	LOG_INFO  << adbase::Fmt("%d test", 45);

	while(true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));		
	}
	return 0;
}
