#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_TCPCONNECTION_HPP_
#define ADBASE_NET_TCPCONNECTION_HPP_

#include <adbase/Net/Types.hpp>

struct tcp_info;
namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

namespace detail {
void eventCallback(struct bufferevent* bev, short events, void* ptr);
void readCallback(struct bufferevent* bev, void* ptr);
void writeCallback(struct bufferevent* bev, void* ptr);
}

class InetAddress;
class Socket;
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(EventBasePtr& base, int sockfd, const InetAddress& localAddr, 
				  const InetAddress& peerAddr, int connId, const RemoveConnectionCallback& removeCb);
	~TcpConnection();	

	const InetAddress& localAddress() const {
		return _localAddr;	
	}

	const InetAddress& peerAddress() const {
		return _peerAddr;	
	}

	void init();
	void eventCallback(short events);
	void readCallback();
	void writeCallback();
	bool getTcpInfo(struct tcp_info*) const;
	std::string getTcpInfoString() const;
	void send(const void* message, size_t len);
	void send(std::string& message);
	void shutdown();
	void setTcpNoDelay(bool on);
	void setContext(void *data) {
		_context = data;	
	}

	void* getContext() {
		return _context;	
	}

	bool connected() const {
		return _state == kConnected;
	}

	bool disconnected() const {
		return _state == kDisconnected;
	}

	void setMessageCallback(const MessageCallback& cb) {
		_messageCallback = cb;	
	}
	void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
		_writeCompleteCallback = cb;	
	}
	void setConnectionCookiePrefix(const ConnectionCookiePrefix& cp) {
		_connectionCookiePrefix = cp;	
	}

	const std::string getCookie();

private:
	enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
	EventBasePtr& _base;
	Socket _socket;
	const InetAddress _localAddr;
	const InetAddress _peerAddr;
	BufferEventPtr _bufferEvent;		
	int _connId;
	bool _isWriting;
	StateE _state;
	RemoveConnectionCallback _removeCallback;
	MessageCallback _messageCallback;
	WriteCompleteCallback _writeCompleteCallback;
	ConnectionCookiePrefix _connectionCookiePrefix;
	void* _context;
	void setState(StateE s) { 
		_state = s;
	}
};

/*@}*/

}

#endif
