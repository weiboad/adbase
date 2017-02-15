#ifndef ADBASE_LOGGING_HPP_
#define ADBASE_LOGGING_HPP_

/** 
 * @addtogroup logging adbase::Logging
 *
 * @brief adbase 日志处理工具库(使用方法见该模块的描述信息)
 *
 * @par 使用方法
 *
 * @note 请勿直接引用对应头文件，adbase::Logging 模块统一引用<adbase/Logging.hpp> 文件
 *
 * @par Example
 * @code
 * #include <adbase/Logging.hpp>
 *
 * .....
 * LOG_TRACE << "trace test";
 * LOG_DEBUG << "debug test";
 * LOG_INFO  << "info test";
 * LOG_ERROR << 45;
 * .....
 * @endcode
 *
 * @par 日志写入
 *
 * 日志写入是分日志等级通过宏替换来做，分为 TRACE, DEBUG, INFO, WARN, ERROR, FATAL, SYSERR, SYSFATAL
 *
 * 通过 LOG_* 方式写入
 * 
 * 默认是 INFO 级别，如果需要修改系统 LOG 级别可以通过两种方式修改
 *
 * @li 通过启动的时候设置环境变量 ADINF_LOG_TRACE=1 或者 ADINF_LOG_DEBUG=1 来设置
 * @li 通过日志库提供的函数动态设置
 *
 * @par 动态修改日志等级
 * @code
 * adbase::Logger::setLogLevel(adbase::Logger::TRACE);
 * @endcode
 *
 * @par 日志落地
 * 
 * 日志落地通过 setOutput回调函数来决定落地方式，默认是写入到 stdout 中， 日志库还提供了一种异步写入到文件的落地方式
 * @code
 * adbase::AsyncLogging* glogger = nullptr;
 * void asyncLogger(const char* msg, int len) {
 *    if (glogger != nullptr) {
 *    	glogger->append(msg, len);
 *    }
 * }
 * static void killSignalMaster(const int sig) {
 *     (void)(sig);
 *    
 *     LOG_ERROR << sig;
 *     if (glogger != nullptr) {
 *    	LOG_INFO << "Stop...";
 *    	delete glogger;
 *    	glogger = nullptr;
 *     }
 *    
 *    /// 给进程组发送SIGTERM信号，结束子进程
 *    kill(0, SIGTERM);
 *    
 *    exit(0);
 * }
 * 
 * int main() {
 * 	/// 处理kill信号 
 * 	signal (SIGINT, killSignalMaster);
 * 	signal (SIGKILL, killSignalMaster);
 * 	signal (SIGQUIT, killSignalMaster);
 * 	signal (SIGTERM, killSignalMaster);
 * 	signal (SIGHUP, killSignalMaster);
 * 	signal (SIGSEGV, killSignalMaster);
 * 	adbase::AsyncLogging*logger = new adbase::AsyncLogging("./test", 5 * 1024 * 1024);
 * 	glogger = logger;
 * 	adbase::Logger::setOutput(&asyncLogger);
 * 	glogger->start();
 * 	
 * 	// something process...
 * 	while(true) {
 * 		std::this_thread::sleep_for(std::chrono::seconds(1));
 * 	}
 * 	return 0;
 * }
 * @endcode
 * @note 异步落地文件在进程终止的时候一定要保证落地对象正常销毁，保证所有的写入日志都落地到磁盘，一般的做法是在捕获信号回调函数中 delete 掉落地对象，保证落地对象在析构的时候正常调用 stop 函数。
 */
/*@{*/ 

#include <adbase/Logging/LogStream.hpp>
#include <adbase/Logging/Logging.hpp>
#include <adbase/Logging/LogFile.hpp>
#include <adbase/Logging/AsyncLogging.hpp>

/*@}*/ 

#endif
