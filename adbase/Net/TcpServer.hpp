#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_TCPSERVER_HPP_
#define ADBASE_NET_TCPSERVER_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Net/Types.hpp>
#include <thread>
#include <vector>
#include <atomic>
#include <unordered_map>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

class InetAddress;
class Acceptor;
class TcpServer {
public:
	TcpServer(EventBasePtr& base, InetAddress& listenAddr, const std::string& name);	
	~TcpServer();
    void threadFunc(int id);
	void start(int threadNum);
	void stop();
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
	static void deleteThread(std::thread *t);
	void newConnection(const InetAddress& remoteAddr, int connfd);

private:
	EventBasePtr& _base;
	InetAddress& _listenAddr;
	std::string _name;
	int _threadNum;
	std::atomic<bool> _started;
	Acceptor _acceptor;
	ConnectionCallback _connectionCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	CloseCallback _closeCallback;
	typedef std::unique_ptr<std::thread, decltype(&TcpServer::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool Threads;
	std::unordered_map<int, std::vector<int>> _pipeMap;
	std::unordered_map<int, Queue<ConnectionBaseInfo>> _connections;
};

/*@}*/

}

#endif
