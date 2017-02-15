#if !defined ADBASE_MC_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_MC_CLIENTHANDLER_HPP_
#define ADBASE_MC_CLIENTHANDLER_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Net.hpp>

namespace adbase {

/**
 * @addtogroup mc adbase::Mc
 */
/*@{*/

namespace mc {
class ClientHandler;
typedef std::function<void (const std::string&, Buffer*, ClientHandler*)> GetCallback; 
class ClientHandler {
public:
	ClientHandler(EventBasePtr& base, const std::string& hostname, int port, Timer* timer, bool isRetry = false);
	~ClientHandler();
	void onConnection(const TcpConnectionPtr& conn);
	void onMessage(const TcpConnectionPtr& conn, evbuffer* evbuf, Timestamp reciveTime);

	void setGetCallback(const GetCallback& cb) {
		_getCallback = cb;	
	}

	void start();
	void stop();
	bool get(const std::string& key);

	void disableBinary() {
		_binary = false;	
	}

private:
	enum GetStates { getHeaderState, getDataState, getEndState };
	EventBasePtr& _base;
	std::string _hostname;
	int _port;
	Timer* _timer;
	bool _isRetry;
	bool _binary;
	TcpClient* _client;
	GetCallback _getCallback;
	Buffer _cmdCache;  // 在还未连接上时发送的命令，需要缓存起来
	Buffer _resultBuffer; // get 操作存储key 
	Buffer _bodyBuffer; // get 操作存储 body
	GetStates _currentGetCmd;
	ssize_t _getDataSize;
	TcpConnectionPtr _connnection;

	void processDataBinary(evbuffer* evbuf);
	void processDataAscii(evbuffer* evbuf);
};

}	

/*@}*/

}
#endif
