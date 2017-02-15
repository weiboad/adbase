#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_STRING_HPP_
#define ADBASE_UTILITY_STRING_HPP_

#include <string>
#include <cstring>
#include <vector>

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/

/// 从字符串左边 trim
std::string leftTrim(const std::string &str, const char* trimStr = nullptr);

/// 从字符串右边 trim
std::string rightTrim(const std::string &str, const char* trimStr = nullptr);

/// 从字符串 trim
std::string trim(const std::string &str, const char* trimStr = nullptr);

/// 拆分字符串
/**
 *
 * @param s 要拆分的字符串
 * @param c 分隔符
 * @param isTrim 是否将拆分的字符串做 trim 操作
 *
 * @returns 拆分后的字符串集合
 *
 * @throws none
 *
 * @par Example
 * @code std::string a = "a1 a2 a3";
 *  std::vector<std::string> b = adbase::explode(a); @endcode
 */
const std::vector<std::string> explode(const std::string& s, const char& c, bool isTrim = false);

/// 替换字符串
/**
 *
 * @param search 要替换搜索的字符串
 * @param replace 要替换的子字符串
 * @param subject 替换文本 
 * @param count 替换的次数 
 *
 * @returns 替换后的字符串
 *
 * @throws none
 *
 * @par Example
 * @code std::string a = "a1 a2 a3";
 * int count;
 *  std::string b = adbase::replace("a1", "a2", a, count); @endcode
 */
const std::string replace(const std::string& search,
						   const std::string& replace, 
						   const std::string& subject, 
						   int& count);

/*@}*/

}

#endif
