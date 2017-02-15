#ifndef ADBASE_MC_HPP_
#define ADBASE_MC_HPP_

/** 
 * @defgroup mc adbase::Mc
 *
 * @brief adbase Memcache 协议工具库(使用方法见该模块的描述信息)
 *
 * @par 使用方法
 *
 * @note 请勿直接引用对应头文件，adbase::Mc 模块统一引用<adbase/Mc.hpp> 文件
 *
 * @par Example
 * 
 * @code
 * 
 * #include <adbase/Logging.hpp>
 * #include <adbase/Net.hpp>
 * #include <adbase/Mc.hpp>
 * #include <signal.h>
 * 
 * adbase::EventLoop* gEventLoop = nullptr;
 * 
 * // {{{ class McProcessor
 * 
 * class McProcessor {
 * public:
 *     McProcessor() {}
 *     ~McProcessor() {}
 * 
 *     adbase::mc::ProtocolBinaryResponseStatus get(const void* key,
 *             uint16_t keylen,
 *             adbase::Buffer *data) {
 *         std::string keyData(static_cast<const char*>(key), static_cast<size_t>(keylen));
 *         data->append(static_cast<const char*>(key), static_cast<size_t>(keylen));
 *         LOG_INFO << "Mc GET key:" << keyData;
 *         return adbase::mc::PROTOCOL_BINARY_RESPONSE_SUCCESS;
 *     }
 * };
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
 *     gEventLoop = new adbase::EventLoop();
 * 
 *     McProcessor mcProcessor;
 *     adbase::InetAddress mcAddr("0.0.0.0", 10012);
 *     adbase::TcpServer mcServer(gEventLoop->getBase(), mcAddr, "test");
 *     adbase::mc::Interface mcInterface;
 * 
 *     mcInterface.setGetHandler(std::bind(&McProcessor::get, &mcProcessor, std::placeholders::_1,
 *                 std::placeholders::_2, std::placeholders::_3));
 *     adbase::mc::Handler mcHandler(&mcInterface);
 *     mcServer.setConnectionCallback(std::bind(&adbase::mc::Handler::onConnection, &mcHandler,
 *                 std::placeholders::_1));
 *     mcServer.setCloseCallback(std::bind(&adbase::mc::Handler::onClose, &mcHandler, std::placeholders::_1));
 *     mcServer.setMessageCallback(std::bind(&adbase::mc::Handler::onMessage, &mcHandler, std::placeholders::_1,
 *                 std::placeholders::_2, std::placeholders::_3));
 *     mcServer.start(1);
 * 
 *     gEventLoop->start();
 * 
 *     return 0;
 * }
 * @endcode
 */
/*@{*/ 

#include <adbase/Mc/Context.hpp>
#include <adbase/Mc/Ascii.hpp>
#include <adbase/Mc/Binary.hpp>
#include <adbase/Mc/Interface.hpp>
#include <adbase/Mc/Handler.hpp>
#include <adbase/Mc/ClientHandler.hpp>

/*@}*/ 

#endif
