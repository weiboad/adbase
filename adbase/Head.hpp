#ifndef ADBASE_HEAD_HPP_
#define ADBASE_HEAD_HPP_

/** 
 * @defgroup head adbase::Head
 *
 * @brief adbase AdHead 协议工具库(使用方法见该模块的描述信息)
 *
 * @par 协议介绍
 * @code 
 *
 * 
 *  Byte/     0       |       1       |       2       |       3       |
 * 	/              |               |               |               |
 *    |0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|0 1 2 3 4 5 6 7|
 *    +---------------+---------------+---------------+---------------+
 *   0| Magic         | DataType      | MasterVersion | SecondVersion |
 *    +---------------+---------------+---------------+---------------+
 *   4| Status                        | Opaque                        |
 *    +---------------+---------------+---------------+---------------+
 *   8| Appkey                                                        |
 *    +---------------+---------------+---------------+---------------+
 *  12| Total body length                                             |
 *    +---------------+---------------+---------------+---------------+
 *  16| Reserved                                                      |
 *    |                                                               |
 *    +---------------+---------------+---------------+---------------+
 *    Total 24 bytes
 * @endcode
 *
 * @par 使用方法
 *
 * @note 请勿直接引用对应头文件，adbase::Head 模块统一引用<adbase/Head.hpp> 文件
 *
 * @par Example
 * 
 * 加载配置支持从字符串中加载，或者从文件中加载两种方式
 *
 * @code
 *
 * #include <adbase/Logging.hpp>
 * #include <adbase/Net.hpp>
 * #include <adbase/Head.hpp>
 * #include <signal.h>
 * 
 * adbase::EventLoop* gEventLoop = nullptr;
 * 
 * // {{{ class HeadProcessor
 * 
 * class HeadProcessor {
 * public:
 *     HeadProcessor() {}
 *     ~HeadProcessor() {}
 *     adbase::head::ProtocolBinaryResponseStatus readHandler(adbase::head::ProtocolBinaryDataType datatype, const void *data, ssize_t datalen,
 *                                                            adbase::head::ProtocolBinaryDataType* resDataType,
 *                                                            adbase::Buffer* responseData) {
 *             (void)data;
 *             (void)datatype;
 *             (void)datalen;
 *             *resDataType = adbase::head::PROTOCOL_BINARY_TYPE_JSON;
 *             std::unordered_map<std::string, std::string> procs = adbase::procStats();
 *             std::string result = "{";
 *             for (auto &t : procs) {
 *                 result +="\"" + t.first + "\":\"" + t.second + "\",";
 *             }
 *             result = adbase::rightTrim(result, ",");
 *             result += "}";
 *             responseData->append(result);
 *             return adbase::head::PROTOCOL_BINARY_RESPONSE_SUCCESS;
 *     }
 * };
 * 
 * // }}}
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (gEventLoop != nullptr) {
 *         gEventLoop->stop();
 *         delete gEventLoop;
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
 *     // 处理kill信号 
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
 *     gEventLoop = new adbase::EventLoop();
 * 
 *     HeadProcessor headProcessor;
 *     adbase::InetAddress headAddr("0.0.0.0", 10011);
 *     adbase::TcpServer headServer(gEventLoop->getBase(), headAddr, "test");
 *     adbase::head::Interface headInterface;
 *     headInterface.setReadHandler(std::bind(&HeadProcessor::readHandler, &headProcessor, std::placeholders::_1,
 *                 std::placeholders::_2, std::placeholders::_3,
 *                 std::placeholders::_4, std::placeholders::_5));
 *     adbase::head::Handler headHandler(&headInterface);
 *     headServer.setConnectionCallback(std::bind(&adbase::head::Handler::onConnection, &headHandler,
 *                 std::placeholders::_1));
 *     headServer.setCloseCallback(std::bind(&adbase::head::Handler::onClose, &headHandler, std::placeholders::_1));
 *     headServer.setMessageCallback(std::bind(&adbase::head::Handler::onMessage, &headHandler, std::placeholders::_1,
 *                 std::placeholders::_2, std::placeholders::_3));
 *     headServer.start(1);
 * 
 *     gEventLoop->start();
 * 
 *     return 0;
 * }
 * 
 * @endcode
 * @note Head Server 需要的数据可以通过 HeadProcessor 类来传递
 */
/*@{*/ 

#include <adbase/Head/Binary.hpp>
#include <adbase/Head/Interface.hpp>
#include <adbase/Head/Handler.hpp>

/*@}*/ 

#endif
