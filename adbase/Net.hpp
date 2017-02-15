#ifndef ADBASE_NET_HPP_
#define ADBASE_NET_HPP_

/** 
 * @defgroup net adbase::Net
 *
 * @brief adbase 网络服务工具库(使用方法见该模块的描述信息)
 *
 * @par 使用方法
 *
 * @note 请勿直接引用对应头文件，adbase::Net 模块统一引用<adbase/Net.hpp> 文件
 *
 * @par Example
 * 
 * 加载配置支持从字符串中加载，或者从文件中加载两种方式
 *
 * @code
 * #include <adbase/Config.hpp>
 * #include <adbase/Logging.hpp>
 * 
 * int main() {
 *     std::string ini = "[http]\nhost=127.0.0.1\nport=80\ntimeout=3\ndaemon=yes\n";
 *     adbase::IniConfig config = adbase::IniParse::load(ini);
 *     //adbase::IniConfig config = adbase::IniParse::loadFile(iniFile);
 *     LOG_INFO << config.getOption("http", "host");
 *     LOG_INFO << config.getOptionUint32("http", "port");
 *     LOG_INFO << config.getOptionUint32("http", "timeout");
 *     LOG_INFO << config.getOptionBool("http", "daemon");
 *     return 0;
 * }
 * @endcode
 * @note INI 配置文件在解析和获取都会抛异常
 */
/*@{*/ 

#include <adbase/Net/SocketsOps.hpp>
#include <adbase/Net/InetAddress.hpp>
#include <adbase/Net/Socket.hpp>
#include <adbase/Net/Types.hpp>
#include <adbase/Net/Acceptor.hpp>
#include <adbase/Net/Connector.hpp>
#include <adbase/Net/Timer.hpp>
#include <adbase/Net/TcpConnection.hpp>
#include <adbase/Net/TcpClient.hpp>
#include <adbase/Net/TcpServer.hpp>
#include <adbase/Net/TcpWorker.hpp>
#include <adbase/Net/EventLoop.hpp>

/*@}*/ 

#endif
