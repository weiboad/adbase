#include <adbase/Logging.hpp>

#include <algorithm>
#include <limits>
#include <cassert>

using namespace adbase;
using namespace adbase::detail;

namespace adbase {
namespace detail {
// {{{ detail
const char digits[] = "9876543210123456789";
static_assert(sizeof(digits) == 20, "Digits size must 20");
const char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof(digitsHex) == 17, "DigitsHex size must 17");
const char* zero = digits + 9;	
template<typename T>
// {{{ size_t convert()
size_t convert(char buf[], T value) {
	T i = value;
	char *p = buf;
	
	do {
		int lsd = static_cast<int>(i % 10);
		i /= 10;
		*p++ = zero[lsd];	
	} while(i != 0);

	if (value < 0) {
		*p++ = '-';	
	}
	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}
// }}}
// {{{ size_t convertHex()

size_t convertHex(char buf[], uintptr_t value) {
	uintptr_t i = value;
	char *p = buf;
	
	do {
		int lsd = static_cast<int>(i % 16);
		i /= 16;
		*p++ = digitsHex[lsd];
	} while (i != 0);	

	*p = '\0';
	std::reverse(buf, p);

	return p - buf;
}

// }}}
template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;
// }}}
}	
// {{{ class FixedBuffer

template<int SIZE>
const char* FixedBuffer<SIZE>::debugString() {
	_current = '\0';
	return _data;	
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart() {
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd() {
}

// }}}
// {{{ class LogStream

void LogStream::staticCheck() {
	static_assert(kMaxNumricSize - 10 > std::numeric_limits<double>::digits10, "Double limit must less 22");
	static_assert(kMaxNumricSize - 10 > std::numeric_limits<long double>::digits10, "Long double limits must less 22");
	static_assert(kMaxNumricSize - 10 > std::numeric_limits<long>::digits10, "Long limits must less 22");
	static_assert(kMaxNumricSize - 10 > std::numeric_limits<long long>::digits10, "Long Long limits must less 22");
}

template<typename T>
void LogStream::formatInteger(T v) {
	if (_buffer.avail() >= kMaxNumricSize) {
		size_t len = convert(_buffer.current(), v);	
		_buffer.add(len);
	}	
}

LogStream& LogStream::operator<<(short v) {
	*this << static_cast<int>(v);	
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
	*this << static_cast<unsigned int>(v);
	return *this;
}

LogStream& LogStream::operator<<(int v) {
	formatInteger(v);
	return *this;	
}

LogStream& LogStream::operator<<(unsigned int v) {
	formatInteger(v);	
	return *this;
}

LogStream& LogStream::operator<<(long v) {
	formatInteger(v);
	return *this;	
}

LogStream& LogStream::operator<<(unsigned long v) {
	formatInteger(v);
	return *this;	
}

LogStream& LogStream::operator<<(long long v) {
	formatInteger(v);
	return *this;	
}

LogStream& LogStream::operator<<(unsigned long long v) {
	formatInteger(v);
	return *this;	
}

LogStream& LogStream::operator<<(const void* p) {
	uintptr_t v = reinterpret_cast<uintptr_t>(p);	
	if (_buffer.avail() >= kMaxNumricSize) {
		char* buf = _buffer.current();		
		buf[0] = '0';
		buf[1] = 'x';
		size_t len = convertHex(buf + 2, v);
		_buffer.add(len + 2);
	}

	return *this;
}

LogStream& LogStream::operator<<(double v) {
	if (_buffer.avail() >= kMaxNumricSize) {
		int len = snprintf(_buffer.current(), kMaxNumricSize, "%.12g", v);		
		_buffer.add(len);
	}	

	return *this;
}

// }}}
// {{{ fmt explicit instantiations

template<typename T>
Fmt::Fmt(const char* fmt, T val) {
	static_assert(std::is_arithmetic<T>::value == true, "Type is invalid");	

	_length = snprintf(_buffer, sizeof _buffer, fmt, val);
	assert(static_cast<size_t>(_length) < sizeof _buffer);
}

template Fmt::Fmt(const char* fmt, char val);
template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);

// }}}
}
