#if !defined ADBASE_LOGGING_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_LOGGING_LOGSTREAM_HPP_
#define ADBASE_LOGGING_LOGSTREAM_HPP_

#include <cstdint>
#include <cstdio>
#include <string>
#include <cstring>
#include <iostream>

namespace adbase {

/**
 * @addtogroup logging adbase::Logging
 */
/*@{*/
namespace detail {
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 4000;	
// {{{ class FixedBuffer
template <int SIZE>
class FixedBuffer {
public:
	FixedBuffer() : _current(_data) {
		setCookie(cookieStart);
	}

	~FixedBuffer() {
		setCookie(cookieEnd);
	}

	void append(const char * buf, size_t len) {
		size_t length = static_cast<size_t>(avail()) - 1;
		if (static_cast<size_t>(avail()) <= len) {
			memcpy(_current, buf, length);	
			_current += length;
		} else {
			memcpy(_current, buf, len);	
			_current += len;
		}
	}

	const char* data() const {
		return _data;	
	}

	int length() const {
		return static_cast<int>(_current - _data);	
	}

	char* current() {
		return _current;	
	}

	int avail() const {
		return static_cast<int>(end() - _current);	
	}
	
	void add(size_t len) {
		_current += len;	
	}

	void reset() {
		_current = _data;	
	}

	void bzero() {
		::bzero(_data, sizeof(_data));	
	}

	const char* debugString();

	void setCookie(void (*cookie)()) {
		_cookie = cookie;	
	}

	std::string toString() const {
		return std::string(_data, length());	
	}

private:	
	const char* end() const {
		return _data + sizeof(_data);	
	}

	static void cookieStart();

	static void cookieEnd();

	void (*_cookie)();

	char _data[SIZE];

	char *_current;
};
// }}}
}
// {{{ class LogStream

class LogStream {
public:	
	typedef LogStream self;
	typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;
	self& operator<<(bool v) {
		_buffer.append(v ? "1" : "0", 1);	
		return *this;	
	}

	self& operator<<(short);
	self& operator<<(unsigned short);
	self& operator<<(int);
	self& operator<<(unsigned int);
	self& operator<<(long);
	self& operator<<(unsigned long);
	self& operator<<(long long);
	self& operator<<(unsigned long long);

	self& operator<<(const void*);

	self& operator<<(float v) {
		*this << static_cast<double>(v);	
		return *this;
	}
	self& operator<<(double);
	
	self& operator<<(char v) {
		_buffer.append(&v, 1);	
		return *this;
	}

	self& operator<<(const char* str) {
		if (str) {
			_buffer.append(str, strlen(str));	
		} else {
			_buffer.append("(null)", 6);	
		}

		return *this;
	}

	self& operator<<(const unsigned char* str) {
		return operator<<(reinterpret_cast<const char*>(str));
	}

	self& operator<<(const std::string& v) {
		_buffer.append(v.c_str(), v.size());
		return *this;
	}
	
	self& operator<<(const Buffer& v) {
		*this << v.toString();
		return *this;	
	}

	void append(const char* data, int len) {
		_buffer.append(data, len);	
	}

	const Buffer& buffer() const {
		return _buffer;	
	}

	void resetBuffer() {
		_buffer.reset();	
	}

private:
	void staticCheck();

	template<typename T>
	void formatInteger(T);

	Buffer _buffer;
	
	static const int kMaxNumricSize = 32;
};

// }}}
// {{{ class Fmt

class Fmt {
public:
	template<typename T>
	Fmt(const char* fmt, T val);

	const char* data() const {
		return _buffer;	
	}		

	int length() const {
		return _length;	
	}

private:
	char _buffer[32];
	int _length;
};

// }}}
inline LogStream& operator<< (LogStream& s, const Fmt& fmt) {
	s.append(fmt.data(), fmt.length());		
	return s;
}
/*@}*/
}
#endif
