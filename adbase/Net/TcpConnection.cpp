#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace detail {
// {{{ void eventCallback()

void eventCallback(struct bufferevent*, short events, void* ptr) {
	TcpConnection* conn = reinterpret_cast<TcpConnection*>(ptr);	
	conn->eventCallback(events);	
}

// }}}
// {{{ void readCallback()

void readCallback(struct bufferevent*, void* ptr) {
	TcpConnection* conn = reinterpret_cast<TcpConnection*>(ptr);	
	conn->readCallback();	
}

// }}}
// {{{ void writeCallback()

void writeCallback(struct bufferevent*, void* ptr) {
	TcpConnection* conn = reinterpret_cast<TcpConnection*>(ptr);	
	conn->writeCallback();	
}

// }}}
// {{{ void defaultConnectionCallback()

void defaultConnectionCallback(const TcpConnectionPtr& conn) {
	LOG_TRACE << conn->localAddress().toIpPort() << " -> "	
			  << conn->peerAddress().toIpPort();
}

// }}}
// {{{ void defaultMessageCallback()

void defaultMessageCallback(const TcpConnectionPtr& conn, evbuffer* buffer, Timestamp receiveTime) {
	(void)conn;
	(void)receiveTime;
	char buf[1024];
	int n = 0;
	while((n = evbuffer_remove(buffer, buf, sizeof(buf))) > 0);
}

// }}}
}
// {{{ TcpConnection::TcpConnection()

TcpConnection::TcpConnection(EventBasePtr& base, int sockfd, const InetAddress& localAddr,
							 const InetAddress& peerAddr, int connId, const RemoveConnectionCallback& removeCb) :
	_base(base),
	_socket(sockfd, false), // 不需要在 Socket 对象析构时关闭连接，一定要等 event 对象析构时关闭连接
	_localAddr(localAddr),
	_peerAddr(peerAddr),
	_connId(connId),
	_isWriting(false),
	_state(kConnected),
	_removeCallback(removeCb) {
	_socket.setKeepAlive(true);
	BufferEventPtr event(bufferevent_socket_new(base.get(), _socket.fd(), BEV_OPT_CLOSE_ON_FREE),
						 std::ptr_fun(&bufferevent_free));
	_bufferEvent = std::move(event);
}

// }}}
// {{{ void TcpConnection::init()

void TcpConnection::init() {
	bufferevent_setcb(_bufferEvent.get(), &detail::readCallback, &detail::writeCallback, &detail::eventCallback, this);
	bufferevent_enable(_bufferEvent.get(), EV_READ | EV_WRITE);
}

// }}}
// {{{ TcpConnection::~TcpConnection()

TcpConnection::~TcpConnection() {
	LOG_TRACE << "TcpConnection connId: " << _connId << " delete.";
}

// }}}
// {{{ bool TcpConnection::getTcpInfo() const

bool TcpConnection::getTcpInfo(struct tcp_info* tcpi) const {
	return _socket.getTcpInfo(tcpi);	
}

// }}}
// {{{ std::string TcpConnection::getTcpInfoString() const

std::string TcpConnection::getTcpInfoString() const {
	char buf[1024];
	buf[0] = '\0';
	_socket.getTcpInfoString(buf, sizeof(buf));
	return buf;
}

// }}}
// {{{ void TcpConnection::send()

void TcpConnection::send(const void* message, size_t len) {
	if (_state != kConnected) {
		LOG_ERROR << "Tcp connect writer has been close.";
		return;	
	}
	if (bufferevent_write(_bufferEvent.get(), message, len) != 0) {
		LOG_ERROR << "ConnId: " << _connId << "bufferevent_write fail.";
	}	
	_isWriting = true;
}

// }}}
// {{{ void TcpConnection::send()

void TcpConnection::send(std::string& message) {
	send(message.c_str(), message.size());	
}

// }}}
// {{{ void TcpConnection::shutdown()

void TcpConnection::shutdown() {
	LOG_DEBUG << "Shutdown connteion";
	if (_isWriting == false) {
		_socket.shutdownWrite();
	} else {
		setState(kDisconnecting);	
	}
}

// }}}
// {{{ void TcpConnection::setTcpNoDelay()

void TcpConnection::setTcpNoDelay(bool on) {
	_socket.setTcpNoDelay(on);	
}

// }}}
// {{{ const std::string TcpConnection::getCookie()

const std::string TcpConnection::getCookie() {
	if (_connectionCookiePrefix) {
		return _connectionCookiePrefix() + "-" + std::to_string(_connId);		
	} else {
		return std::to_string(_connId);	
	}
	//return _worker->getServerName() + "-" + std::to_string(_worker->getWorkerId()) +
	return std::to_string(_connId);	
}

// }}}
// {{{ void TcpConnection::eventCallback()

void TcpConnection::eventCallback(short events) {
	if (events & BEV_EVENT_EOF) {
		LOG_DEBUG << "connection close event";	
	} else if (events & BEV_EVENT_ERROR) {
		LOG_DEBUG << "Event error";	
	} else if (events & BEV_EVENT_TIMEOUT) {
		LOG_DEBUG << "Event timeout";	
	} else {
		LOG_ERROR << "Event error: " << events;	
	}
	LOG_TRACE << "Remove tcp connection.";
	_removeCallback(_connId);
	setState(kDisconnecting);	
}

// }}}
// {{{ void TcpConnection::readCallback()

void TcpConnection::readCallback() {
	LOG_TRACE << "Read event";
	struct evbuffer* evbuf = bufferevent_get_input(_bufferEvent.get());		
	_messageCallback(shared_from_this(), evbuf, Timestamp::now());
}

// }}}
// {{{ void TcpConnection::writeCallback()

void TcpConnection::writeCallback() {
	LOG_DEBUG << "Write event";	
	_isWriting = false;
	if (_writeCompleteCallback) {
		_writeCompleteCallback(shared_from_this());	
	}

	if (_state == kDisconnecting) {
		_socket.shutdownWrite();	
		LOG_TRACE << "Shutdown socket read.";
	}
}

// }}}
}
