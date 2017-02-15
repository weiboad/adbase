#ifndef ADBASE_HTTP_HPP_
#define ADBASE_HTTP_HPP_

/** 
 * @defgroup http adbase::Http
 *
 * @brief adbase Http 协议工具库(使用方法见该模块的描述信息)
 *
 * @par 使用方法
 *
 * @note 请勿直接引用对应头文件，adbase::Http 模块统一引用<adbase/Http.hpp> 文件
 *
 * @par Example
 * 
 * @code
 * 	
 * #include <adbase/Http.hpp>
 * #include <adbase/Logging.hpp>
 * #include <signal.h>
 * 
 * adbase::http::Server* ghttp = nullptr;
 * 
 * // {{{ void request()
 * 
 * void request(adbase::http::Request* request, adbase::http::Response* response, void* data) {
 *     (void)request;
 *     if (data != nullptr) {
 *         std::string* context = reinterpret_cast<std::string*>(data);
 *         LOG_INFO << "Context:" << *context;
 *     } else {
 *         (void)data;
 *     }
 *     response->sendReply("OK", 200, "OK");
 * }
 * 
 * // }}}
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (ghttp != nullptr) {
 *         ghttp->stop();
 *         delete ghttp;
 *         ghttp = nullptr;
 *     }
 *     exit(0);
 * }
 * 
 * // }}}
 * // {{{ static void reloadConf()
 * 
 * static void reloadConf(const int sig) {
 *     (void)sig;
 * }
 * 
 * // }}}
 * // {{{ static void registerSignal()
 * 
 * static void registerSignal() {
 *     /// 忽略Broken Pipe信号 
 *     signal(SIGPIPE, SIG_IGN);
 *     /// 处理kill信号 
 *     signal(SIGINT,  killSignal);
 *     signal(SIGKILL, killSignal);
 *     signal(SIGQUIT, killSignal);
 *     signal(SIGTERM, killSignal);
 *     signal(SIGHUP,  killSignal);
 *     signal(SIGSEGV, killSignal);
 *     signal(SIGUSR1, reloadConf);
 * }
 * 
 * // }}}
 *
 * int main(void) {
 *     registerSignal();
 *     adbase::TimeZone tz(8*3600, "CST");
 *     adbase::Logger::setTimeZone(tz);
 *     adbase::http::Config config("0.0.0.0", 10010, 3);
 *     config.setTimeZone(tz);
 *     config.setServerName("test");
 *     config.setLogDir("logs");
 *     adbase::http::Server* http = new adbase::http::Server(config);
 *     ghttp = http;
 *     std::string test = "test string";
 *     http->registerLocation("/debug", std::bind(request, std::placeholders::_1,
 *                             std::placeholders::_2, std::placeholders::_3), &test);
 *     http->registerLocation("/debug1", std::bind(request, std::placeholders::_1,
 *                     std::placeholders::_2, std::placeholders::_3), nullptr);
 *     http->registerLocation("/debug2", std::bind(request, std::placeholders::_1,
 *                     std::placeholders::_2, std::placeholders::_3), nullptr);
 *     http->start(4);
 * 
 *     // do something
 * 
 *     while (true) {
 *         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 *     }
 *     LOG_INFO << "exit.";
 *     return 0;
 * }
 * 
 * @endcode
 */
/*@{*/ 

#include <adbase/Http/Config.hpp>
#include <adbase/Http/Request.hpp>
#include <adbase/Http/Response.hpp>
#include <adbase/Http/Server.hpp>

/*@}*/ 

#endif
