#ifndef ADBASE_UTILITY_HPP_
#define ADBASE_UTILITY_HPP_

/** 
 * @addtogroup utility adbase::Utility
 *
 * @brief adbase 基础通用工具(使用方法见该模块的描述信息)
 *
 * @par 使用方法
 *
 * @note 请勿直接引用对应头文件，util 模块统一引用<adbase/Utility.hpp> 文件
 *
 *
 * @par Example
 * @code
 * #include <adbase/Utility.hpp>
 *
 * .....
 * const string input = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
 * adbase::Sha1 checksum;
 * checksum.update(input);
 * char hash[20] = {0};
 * checksum.final(hash);
 * .....
 * @endcode
 *
 */
/*@{*/ 

#include <adbase/Utility/Endian.hpp>
#include <adbase/Utility/Buffer.hpp>
#include <adbase/Utility/Sha1.hpp>
#include <adbase/Utility/String.hpp>
#include <adbase/Utility/Serialize.hpp>
#include <adbase/Utility/Timestamp.hpp>
#include <adbase/Utility/Date.hpp>
#include <adbase/Utility/TimeZone.hpp>
#include <adbase/Utility/FileSystem.hpp>
#include <adbase/Utility/ProcessInfo.hpp>
#include <adbase/Utility/Queue.hpp>
#include <adbase/Utility/Sequence.hpp>
#include <adbase/Utility/Singleton.hpp>

/*@}*/ 
#endif
