#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_TCPWORKER_HPP_
#define ADBASE_NET_TCPWORKER_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Net/Types.hpp>
#include <unistd.h>
#include <unordered_map>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

namespace detail {
void pipeHandler(int, short evt, void *arg); 
}
class TcpConnection;
class TcpWorker {
public:
	TcpWorker(int pipe, Queue<ConnectionBaseInfo>* queue, int threadId, std::string& serverName);
	~TcpWorker();
	void start();
	void setConnectionCallback(const ConnectionCallback& cb) {
		_connectionCallback = cb;	
	} 
	void setMessageCallback(const MessageCallback& cb) {
		_messageCallback = cb;	
	}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
		_writeCompleteCallback = cb;	
	}
	void setCloseCallback(const CloseCallback& cb) {
		_closeCallback = cb;	
	}

	int getWorkerId() {
		return _threadId;	
	}

	std::string getServerName() {
		return _name;	
	}

	size_t getConnectionNum() {
		return _connections.size();	
	}

	void removeConnection(int connId);
	std::string connectionCookiePrefix();

	void pipeHandler();
private:
	int _pipe;
	Queue<ConnectionBaseInfo>* _queue;
	int _threadId;
	std::string _name;
	int _connId;
	ConnectionCallback _connectionCallback;	
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	CloseCallback _closeCallback;
	EventBasePtr _base; 
	struct event _pipeEvent;
	std::unordered_map<int, TcpConnectionPtr> _connections;
};

/*@}*/

}
#endif
