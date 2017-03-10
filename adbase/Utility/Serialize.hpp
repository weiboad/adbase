#if !defined ADBASE_UTILITY_HPP_
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_SERIALIZE_HPP_
#define ADBASE_UTILITY_SERIALIZE_HPP_

#include <string>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <sys/types.h>
#include <algorithm>
#include <cmath>
#include <unistd.h>

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/

/// http url encode
/**
 *
 * @param str encode url
 *
 * @returns encode result
 *
 * @throws none
 *
 * @par Example
 * @code
 *  std::string url = "http://www.weibo.com?test=1&test??????";
 *  std::string encode = adbase::urlEncode(url);
 *  std::cout << encode << std::endl;
 *  std::cout << adbase::urlDecode(encode) << std::endl;
 * @endcode
 */
std::string urlEncode(const std::string& str);

/// http url decode
/**
 *
 * @param str decode url
 *
 * @returns decode result
 *
 * @throws none
 *
 * @par Example
 * @code
 *  std::string url = "http://www.weibo.com?test=1&test??????";
 *  std::string encode = adbase::urlEncode(url);
 *  std::cout << encode << std::endl;
 *  std::cout << adbase::urlDecode(encode) << std::endl;
 * @endcode
 */
std::string urlDecode(const std::string& str);

/// base64 encode
/**
 *
 * @param data encode data
 * @param len encode data length
 *
 * @returns encode result
 *
 * @throws none
 *
 * @par Example
 * @code
 * std::string base64 = "test base64";
 * std::string base64encode = adbase::base64Encode(base64.c_str(), base64.size());
 * std::cout << base64encode << std::endl;
 * @endcode
 */
std::string base64Encode(const char* data, size_t len);

/// base64 decode
/**
 *
 * @param data decode result
 * @param dataLen decode data length
 * @param str decode string
 *
 * @returns void
 *
 * @throws none
 *
 * @par Example
 * @code
 * std::string base64 = "test base64";
 * std::string base64encode = adbase::base64Encode(base64.c_str(), base64.size());
 * std::cout << base64encode << std::endl;
 * char base64decode[base64encode.size()];
 * memset(base64decode, 0, base64encode.size());
 * int length = 0;
 * adbase::base64Decode(base64decode, &length, base64encode);
 * std::cout << base64decode << std::endl;
 *
 * @endcode
 */
void base64Decode(char* data, int* dataLen, const std::string& str);

/// char to hex
unsigned char toHex(unsigned char x);

/// hex to char
unsigned char fromHex(unsigned char x);

///  16进制字符串转化 bits
/**
 *
 * @param str 需要转化的 16 进制字符串
 * @param buffer 转化结果
 *
 * @returns void
 *
 * @note buffer 内存分配的长度可以根据 str 长度的 1/2 来分配
 *
 * @par Example
 * @code
 * std::string str = "1d23456adbcdef";
 * char buffer[str.size() / 2];
 * adbase::hexStringToBytes(str, buffer);
 * cout << adbase::bytesToHexString(buffer, str.size() / 2);
 * @endcode
 */
void hexStringToBytes(const std::string& str, char* buffer);

/// bits 转化为16进制可见字符
/**
 *
 * @param buffer 要转化的 bits
 * @param length 要转化的 bits 的长度
 *
 * @returns std::string 转化结果
 *
 * @par Example
 * @code
 * std::string str = "1d23456adbcdef";
 * char buffer[str.size() / 2];
 * adbase::hexStringToBytes(str, buffer);
 * cout << adbase::bytesToHexString(buffer, str.size() / 2);
 * @endcode
 */
std::string bytesToHexString(const char* buffer, size_t length);

std::string base62Encode(uint64_t id);

uint64_t base62Decode(const std::string& str);

// base62 调用函数
std::string encodeSegment(std::string id);
// base62 调用函数
std::string decodeSegment(const std::string& id);

/*@}*/

}

#endif
