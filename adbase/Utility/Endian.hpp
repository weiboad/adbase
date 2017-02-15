#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_ENDIAN_HPP_
#define ADBASE_UTILITY_ENDIAN_HPP_

#include <stdint.h>
#include <endian.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32))
    #include <byteswap.h>
	# if __BYTE_ORDER == __LITTLE_ENDIAN
	#  define htobe16(x) __bswap_16 (x)
	#  define htole16(x) (x)
	#  define be16toh(x) __bswap_16 (x)
	#  define le16toh(x) (x)
	
	#  define htobe32(x) __bswap_32 (x)
	#  define htole32(x) (x)
	#  define be32toh(x) __bswap_32 (x)
	#  define le32toh(x) (x)
	
	#  define htobe64(x) __bswap_64 (x)
	#  define htole64(x) (x)
	#  define be64toh(x) __bswap_64 (x)
	#  define le64toh(x) (x)
	# else
	#  define htobe16(x) (x)
	#  define htole16(x) __bswap_16 (x)
	#  define be16toh(x) (x)
	#  define le16toh(x) __bswap_16 (x)
	
	#  define htobe32(x) (x)
	#  define htole32(x) __bswap_32 (x)
	#  define be32toh(x) (x)
	#  define le32toh(x) __bswap_32 (x)
	
	#  define htobe64(x) (x)
	#  define htole64(x) __bswap_64 (x)
	#  define be64toh(x) (x)
	#  define le64toh(x) __bswap_64 (x)
	# endif
#endif

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/

// the inline assembler code makes type blur,
// so we disable warnings for a while.
#if defined __GNUC_MINOR__ >= 6
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"

/// uint64_t 的整形数字由机器字节序转化为网络字节序
/**
 *
 * @param host64 需要转化的数据
 *
 * @returns 转化为网络字节序的数据
 *
 * @throws none
 *
 * @par Example
 * @code uint64_t a = 23;
 *  uint64_t b = adbase::hostToNetwork64(a); @endcode
 */
inline uint64_t hostToNetwork64(uint64_t host64) {
	return htobe64(host64);	
}

/// uint32_t 的整形数字由机器字节序转化为网络字节序
/**
 *
 * @param host32 需要转化的数据
 *
 * @returns 转化为网络字节序的数据
 *
 * @throws none
 *
 * @par Example
 * @code uint32_t a = 23;
 *  uint32_t b = adbase::hostToNetwork32(a); @endcode
 */
inline uint32_t hostToNetwork32(uint32_t host32) {
	return htobe32(host32);	
}

/// uint16_t 的整形数字由机器字节序转化为网络字节序
/**
 *
 * @param host16 需要转化的数据
 *
 * @returns 转化为网络字节序的数据
 *
 * @throws none
 *
 * @par Example
 * @code uint16_t a = 23;
 *  uint16_t b = adbase::hostToNetwork16(a); @endcode
 */
inline uint16_t hostToNetwork16(uint16_t host16) {
	return htobe16(host16);	
}

/// uint64_t 的整形数字由网络字节序转化为机器字节序
/**
 *
 * @param net64 需要转化的数据
 *
 * @returns 转化为机器字节序的数据
 *
 * @throws none
 *
 * @par Example
 * @code uint64_t a = 23;
 *  uint64_t b = adbase::networkToHost64(a); @endcode
 */
inline uint64_t networkToHost64(uint64_t net64) {
	return be64toh(net64);	
}

/// uint32_t 的整形数字由网络字节序转化为机器字节序
/**
 *
 * @param net32 需要转化的数据
 *
 * @returns 转化为机器字节序的数据
 *
 * @throws none
 *
 * @par Example
 * @code uint32_t a = 23;
 *  uint32_t b = adbase::networkToHost32(a); @endcode
 */
inline uint32_t networkToHost32(uint32_t net32) {
	return be32toh(net32);	
}

/// uint16_t 的整形数字由网络字节序转化为机器字节序
/**
 *
 * @param net16 需要转化的数据
 *
 * @returns 转化为机器字节序的数据
 *
 * @throws none
 *
 * @par Example
 * @code uint16_t a = 23;
 *  uint16_t b = adbase::networkToHost16(a); @endcode
 */
inline uint16_t networkToHost16(uint16_t net16) {
	return be16toh(net16);	
}

#if __GNUC_MINOR__ >= 6
#pragma GCC diagnostic pop
#endif
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wold-style-cast"

/*@}*/

}	
#endif
