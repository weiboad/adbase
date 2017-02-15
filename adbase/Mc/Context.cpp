#include <adbase/Mc.hpp>
#include <adbase/Utility.hpp>

namespace adbase {
namespace mc {
// {{{ Context::Context()

Context::Context() {
}

// }}}
// {{{ Context::~Context()

Context::~Context() {
}

// }}}
// {{{ void Context::setAsciiBuffer()

void Context::setAsciiBuffer(const char* data, size_t len) {
	_asciiBuffer.append(data, len);	
}

// }}} 
// {{{ const char* Context::getAsciiBuffer()

const char* Context::getAsciiBuffer(size_t& len) {
	len = _asciiBuffer.readableBytes();
	_asciiBuffer.retrieve(len);
	return _asciiBuffer.peek();
}

// }}} 
// {{{ int Context::getAsciiBufferSize()

int Context::getAsciiBufferSize() {
	return static_cast<int>(_asciiBuffer.readableBytes());	
}

// }}}
}
}

