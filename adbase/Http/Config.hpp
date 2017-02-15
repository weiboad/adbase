#if !defined ADBASE_HTTP_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_HTTP_CONFIG_HPP_
#define ADBASE_HTTP_CONFIG_HPP_

#include <adbase/Utility.hpp>
#include <ostream>
#include <string>

namespace adbase {

/**
 * @addtogroup http adbase::Http
 */
/*@{*/

namespace http {

class Config {
public:
	Config(std::string address = "127.0.0.1", int port = 10000, int timeout = 30);
	Config(const Config& config);
	const std::string& getBindAddress() const { return _bindAddress;}
	int getBindPort() const { return _bindPort; }
	int getTimeout() const { return _requestTimeout; }
	int getStatInterval() const { return _statInterval; }
	const TimeZone& getTimeZone() { return _timeZone; }
	const std::string& getLogFormat() const { return _logFormat; }
	const std::string& getServerName() const { return _serverName; }
	const std::string& getLogDir() const { return _logDir; }
	int getLogRollSize() const { return _logRollSize; }
	void setBindAddress(std::string address);	
	void setLogFormat(std::string logFormat);	
	void setTimeZone(TimeZone& zone);	
	void setBindPort(int port);	
	void setTimeout(int timout);
	void setLogRollSize(int size);
	void setStatInterval(int interval);
	void setLogDir(const std::string dir);
	void setServerName(const std::string name);

	friend std::ostream & operator<<(std::ostream &os, const Config &config); 

	virtual ~Config() {}
private:
	std::string _bindAddress;
	int _bindPort;	
	int _requestTimeout;
	int _statInterval = 60000;
	TimeZone _timeZone;
	std::string _logFormat = "$http_x_forwarded_for $remote_addr - - $remote_user [$time_local] $request_time $upstream_response_time \"$request\" $status $body_bytes_sent \"$http_referer\" \"$http_user_agent\"";
	std::string _logDir;
	int _logRollSize = 50 * 1024 * 1024;
	std::string _serverName = "adbase";
};

}

/*@}*/

}

#endif
