#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_PROCESSINFO_HPP_
#define ADBASE_UTILITY_PROCESSINFO_HPP_

#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/

/// 获取进程ID
pid_t pid();	

/// 获取进程ID
std::string pidString();

/// 获取主机名
std::string hostname();

/// 获取主机的 ip 列表
std::unordered_map<std::string, std::unordered_map<std::string, std::string>> ifconfig();

/// 获取进程名
std::string procname();

/// 获取进程名
std::string procname(const std::string& stat);

/// 获取进程状态（可视化版）
std::string procStatus();

/// 获取进程状态
std::string procStat();

/// 获取当前进程的状态
const std::unordered_map<std::string, std::string> procStats();

/// 获取当前进程打开的文件数
int procFdNum();

/*@}*/
}
#endif
