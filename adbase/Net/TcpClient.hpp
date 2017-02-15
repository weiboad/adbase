#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_TCPCLIENT_HPP_
#define ADBASE_NET_TCPCLIENT_HPP_

#include <mutex>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

class InetAddress;
class Timer;
class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;
class TcpClient {
public:
	TcpClient(EventBasePtr& base, const InetAddress& serverAddr, const std::string& name, Timer* timer);	
	TcpClient(EventBasePtr& base, const std::string& hostname, int port, const std::string& name, Timer* timer);	
	~TcpClient();

	void connect();
	void disconnect();
	void stop();

	TcpConnectionPtr connection() const {
		std::lock_guard<std::mutex> lk(_mut);
		return _connection;
	}

	EventBasePtr& getLoop() const {
		return _base;	
	}
	
	bool retry() const {
		return _retry;	
	}

	void enableRetry() {
		_retry = true;	
	}

	const std::string& name() const {
		return _name;	
	}

	void setConnectionCallback(const ConnectionCallback& cb) {
		_connectionCallback = cb;
	}
	void setMessageCallback(const MessageCallback& cb) {
		_messageCallback = cb;
	}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
		_writeCompleteCallback = cb;
	}

private:
	mutable std::mutex _mut;
	void newConnection(int sockfd);
	void removeConnection(int connId);

	EventBasePtr& _base;
	ConnectorPtr _connector;
	const std::string _name;
	Timer* _timer;
	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	bool _retry;
	bool _connect;
	int _nextConnId;
	TcpConnectionPtr _connection;
};
	
/*@}*/

}

#endif
