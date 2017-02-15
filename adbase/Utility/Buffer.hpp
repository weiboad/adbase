#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_BUFFER_HPP_
#define ADBASE_UTILITY_BUFFER_HPP_

#include <algorithm>
#include <vector>
#include <cstring>
#include <cassert>
#include <adbase/Utility/Endian.hpp>

namespace adbase {

/** 
 * @addtogroup utility adbase::Utility
 */
/*@{*/ 

/** 
 *  @par 内部结构
 *  @code
 *  +-------------------+------------------+------------------+
 *  | prependable bytes |  readable bytes  |  writable bytes  |
 *  |                   |     (CONTENT)    |                  |
 *  +-------------------+------------------+------------------+
 *  |                   |                  |                  |
 *  0      <=      readerIndex   <=   writerIndex    <=     size
 *  @endcode

 *  @brief Buffer 可以存取多种类型的数据，保证二进制安全，并且内存可以自动分配
 */
class Buffer {
public:
	static const size_t kInitialSize = 1024;
	
	explicit Buffer(size_t initialSize = kInitialSize) : 
		_buffer(initialSize),
		_writeIndex(0),
		_readIndex(0) {
	}

	/// 获取 buffer 长度
	size_t readableBytes() const {
		return _writeIndex - _readIndex;
	}

	/// 获取 buffer 可写长度
	size_t writableBytes() const {
		return _buffer.size() - _writeIndex;
	}

	/// buffer 已读取向前可写长度
	size_t prependableBytes() const {
		return _readIndex;
	}

	/// 获取 buffer 指针
	const char* peek() const {
		return begin() + _readIndex;	
	}

	/// 删除指定长度的数据 , 按照当前读取的指针位置向后删除,
	/// 当删除的长度大于 buffer 中的数据时会清空整个 buffer
	void retrieve(size_t len) {
		if (len < readableBytes()) {
			_readIndex += len;	
		} else {
			retrieveAll();	
		}
	}

	/// 清空数据到指定位置 
	void retrieveUntil(const char* end) {
		retrieve(end - peek());	
	}

	/// 清空 int64_t 长度的数据 
	void retrieveInt64() {
		retrieve(sizeof(int64_t));
	}

	/// 清空 int32_t 长度的数据 
	void retrieveInt32() {
		retrieve(sizeof(int32_t));
	}

	/// 清空 int16_t 长度的数据 
	void retrieveInt16() {
		retrieve(sizeof(int16_t));
	}

	/// 清空 int8_t 长度的数据 
	void retrieveInt8() {
		retrieve(sizeof(int8_t));
	}

	/// 删除 buffer 中所有数据
	void retrieveAll() {
		_readIndex = 0;
		_writeIndex = 0;
	}

	/// 将 buffer 转化为 std::string 类型
	std::string retrieveAllAsString() {
		return retrieveAsString(readableBytes());
	}

	/// 将指定长度的 buffer 转化为 std::string 类型
	std::string retrieveAsString(size_t len) {
		std::string result(peek(), len);
		retrieve(len);
		return result;	
	}

	/// 添加 std::string 类型字符串到 buffer 中
	void append(const std::string& str) {
		append(str.data(), str.size());
	}

	/// 添加 char* 到 buffer 中
	void append(const char* /*restrict*/ data, size_t len) {
		ensureWritableBytes(len);
		std::copy(data, data+len, beginWrite());
		hasWritten(len);
	}

	/// 添加 void* 到 buffer 中
	void append(const void* /*restrict*/ data, size_t len) {
		append(static_cast<const char*>(data), len);
	}

	/// 确保 buffer 的写入空间够用，如果不够自动分配内存 
	void ensureWritableBytes(size_t len) {
		if (writableBytes() < len) {
			makeSpace(len);
		}
	}

	const char* beginWrite() const {
		return begin() + _writeIndex;
	}

	char* beginWrite() {
		return begin() + _writeIndex;
	}

	void hasWritten(size_t len) {
		_writeIndex += len;	
	}

	///
	/// Append int64_t using network endian
	///
	void appendInt64(int64_t x) {
		int64_t be64 = hostToNetwork64(x);
		append(&be64, sizeof be64);
	}

	///
	/// Append int32_t using network endian
	///
	void appendInt32(int32_t x) {
		int32_t be32 = hostToNetwork32(x);
		append(&be32, sizeof be32);
	}

	void appendInt16(int16_t x) {
		int16_t be16 = hostToNetwork16(x);
		append(&be16, sizeof be16);
	}

	void appendInt8(int8_t x) {
		append(&x, sizeof x);
	}

	///
	/// Read int64_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int64_t readInt64() {
		int64_t result = peekInt64();
		retrieveInt64();
		return result;
	}

	///
	/// Read int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t readInt32() {
		int32_t result = peekInt32();
		retrieveInt32();
		return result;
	}

	int16_t readInt16() {
		int16_t result = peekInt16();
		retrieveInt16();
		return result;
	}

	int8_t readInt8() {
		int8_t result = peekInt8();
		retrieveInt8();
		return result;
	}

	///
	/// Peek int64_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int64_t)
	int64_t peekInt64() const {
		assert(readableBytes() >= sizeof(int64_t));
		int64_t be64 = 0;
		::memcpy(&be64, peek(), sizeof be64);
		return networkToHost64(be64);
	}

	///
	/// Peek int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t peekInt32() const {
		assert(readableBytes() >= sizeof(int32_t));
		int32_t be32 = 0;
		::memcpy(&be32, peek(), sizeof be32);
		return networkToHost32(be32);
	}

	int16_t peekInt16() const {
		assert(readableBytes() >= sizeof(int16_t));
		int16_t be16 = 0;
		::memcpy(&be16, peek(), sizeof be16);
		return networkToHost16(be16);
	}

	int8_t peekInt8() const {
		assert(readableBytes() >= sizeof(int8_t));
		int8_t x = *peek();
		return x;
	}
	
	///
	/// Prepend int64_t using network endian
	///
	void prependInt64(int64_t x) {
		int64_t be64 = hostToNetwork64(x);
		prepend(&be64, sizeof be64);
	}

	///
	/// Prepend int32_t using network endian
	///
	void prependInt32(int32_t x) {
		int32_t be32 = hostToNetwork32(x);
		prepend(&be32, sizeof be32);
	}

	void prependInt16(int16_t x) {
		int16_t be16 = hostToNetwork16(x);
		prepend(&be16, sizeof be16);
	}

	void prependInt8(int8_t x) {
		prepend(&x, sizeof x);
	}

	void prepend(const void* /*restrict*/ data, size_t len) {
		assert(len <= prependableBytes());
		_readIndex -= len;
		const char* d = static_cast<const char*>(data);
		std::copy(d, d + len, begin() + _readIndex);
	}

	void shrink() {
		_buffer.shrink_to_fit();
	}

	size_t internalCapacity() const {
		return _buffer.capacity();
	}


private:
	char* begin() {
		return &*_buffer.begin();	
	}

	const char* begin() const {
		return &*_buffer.begin();	
	}

	void makeSpace(size_t len) {
		if (writableBytes() + prependableBytes() < len) {
			_buffer.resize(_writeIndex + len);
		} else {
			// move readable data to the front, make space inside buffer
			size_t readable = readableBytes();
			std::copy(begin() + _readIndex,
					begin() + _writeIndex,
					begin());
			_readIndex = 0;
			_writeIndex = _readIndex + readable;
		}
	}

	std::vector<char> _buffer;
	size_t _writeIndex;
	size_t _readIndex;
};
/*@}*/ 
}

#endif
