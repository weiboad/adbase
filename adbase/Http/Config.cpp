#include <adbase/Http.hpp>
#include <adbase/Logging.hpp>

namespace adbase {

namespace http {
// {{{ Config::Config()

Config::Config(std::string address, int port, int timeout) : 
	_bindAddress(address),
	_bindPort(port),
	_requestTimeout(timeout) {
}

// }}}
// {{{ Config::Config()

Config::Config(const Config& other) {
	_bindAddress = other._bindAddress;
	_bindPort    = other._bindPort;
	_timeZone    = other._timeZone;
	_logRollSize = other._logRollSize;
	_logFormat   = other._logFormat;
	_logDir      = other._logDir;
	_serverName  = other._serverName;
	_requestTimeout = other._requestTimeout;
}

// }}}
// {{{ void Config::setBindAddress()

void Config::setBindAddress(std::string address) {
	_bindAddress = address;	
}

// }}}
// {{{ void Config::setLogFormat()

void Config::setLogFormat(std::string logFormat) {
	_logFormat = logFormat;	
}

// }}}
// {{{ void Config::setBindPort()

void Config::setBindPort(int port) {
	_bindPort = port;	
}

// }}}
// {{{ void Config::setTimeout()

void Config::setTimeout(int timeout) {
	_requestTimeout = timeout;	
}

// }}}
// {{{ void Config::setStatInterval()

void Config::setStatInterval(int interval) {
	_statInterval = interval;
}

// }}}
// {{{ void Config::setTimeZone()

void Config::setTimeZone(TimeZone& zone) {
	_timeZone = zone;	
}

// }}}
// {{{ void Config::setLogDir()

void Config::setLogDir(const std::string dir) {
	_logDir = dir;	
}

// }}}
// {{{ void Config::setServerName()

void Config::setServerName(const std::string name) {
	_serverName = name;	
}

// }}}
// {{{ void Config::setLogRollSize()

void Config::setLogRollSize(int size) {
	_logRollSize = size;	
}

// }}}
// {{{ std::ostream & operator<<()

std::ostream & operator<<(std::ostream &os, const Config &config) {
	os << "[Object HttpConfig]: bind address->" << config._bindAddress;
	os << ",bind port ->" << config._bindPort << ", server request timeout->" << config._requestTimeout;

	return os;
}

// }}}
}
}
