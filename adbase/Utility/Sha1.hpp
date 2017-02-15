#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_SHA1_HPP_
#define ADBASE_UTILITY_SHA1_HPP_

#include <cstdint>
#include <string>

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 *
 */
/*@{*/

/**
 * @brief 实现 SHA1 算法
 *
 * @par Example
 * @code 
 *  const string input = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
 *  adbase::Sha1 checksum;
 *  checksum.update(input);
 *  std::string hash = checksum.final();
 *  @endcode
 */
class Sha1
{
public:
    Sha1();

	/// 给定一个需要编码的字符串
	/**
	 *
	 * @par Example
	 * @code 
	 *  const string input = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	 *  adbase::Sha1 checksum;
	 *  checksum.update(input);
	 *  std::string hash = checksum.final();
	 *  @endcode
	 */
    void update(const std::string &s);

	/// 给定一个需要编码的字符串
	/**
	 *
	 * @par Example
	 * @code 
	 *  std::ifstream stream(filename.c_str(), std::ios::binary);
	 *  Sha1 checksum;
	 *  checksum.update(stream);
	 *  std::string hash = checksum.final();
	 *  @endcode
	 */
    void update(std::istream &is);

	/// 获取最终编码的字符串
	/**
	 *
	 * @returns 最终编码的字符串
	 *
	 * @throws none
	 *
	 * @par Example
	 * @code 
	 *  const string input = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	 *  adbase::Sha1 checksum;
	 *  checksum.update(input);
	 *  std::string hash = checksum.final();
	 *  @endcode
	 */
    std::string final();

	/// 直接给定文件进行 sha1 编码
	/**
	 *
	 * @returns 最终编码的字符串
	 *
	 * @throws none
	 *
	 * @par Example
	 * @code 
	 *	std::string filename = 'test.txt';
	 *  std::string hash = adbase::Sha1::from_file(filename);
	 *  @endcode
	 */
    static std::string from_file(const std::string &filename);

	/// 获取最终编码的字符串
	/**
	 *
	 * @param result 返回编码的结果(raw)
	 * @returns void
	 *
	 * @throws none
	 *
	 * @par Example
	 * @code 
	 *  const string input = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	 *  adbase::Sha1 checksum;
	 *  checksum.update(input);
	 *  char hash[20] = {0};
	 *  checksum.final(hash);
	 *  @endcode
	 */
    void final(char* result);

	static void hmacSha1(const char* key, size_t keylen, const char* msg, size_t msglen, char* out);

private:
    uint32_t _digest[5];
    std::string _buffer;
    uint64_t _transforms;
};

/*@}*/

}
#endif
