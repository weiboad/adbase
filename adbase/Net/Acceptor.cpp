#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace detail {
// {{{ void acceptHandler()

void acceptHandler(int fd, short evt, void *arg) {
	if (evt & EV_READ) {
		LOG_DEBUG << "Accept connection.";
		Acceptor* accept = reinterpret_cast<Acceptor*>(arg);
		accept->acceptHandler(fd);
	} else {
		LOG_ERROR << "Accept event error.";	
	}
}

// }}}
}
// {{{ Acceptor::Acceptor()

Acceptor::Acceptor(EventBasePtr& base, const InetAddress& listenAddr, bool reuseport) : 
	_base(base),
	_acceptSocket(sockets::createNonblockingOrDie()) {
	_acceptSocket.setReuseAddr(true);
	_acceptSocket.setReusePort(reuseport);
	_acceptSocket.bindAddress(listenAddr);
	_acceptSocket.listen();
	LOG_DEBUG << "Bind: " << listenAddr.toIpPort();
}

// }}}
// {{{ Acceptor::~Acceptor()

Acceptor::~Acceptor() {
}

// }}}
// {{{ void Acceptor::init()

void Acceptor::init() {
	event_set(&_acceptEvent, _acceptSocket.fd(), EV_READ | EV_PERSIST, detail::acceptHandler, this);
	event_base_set(_base.get(), &_acceptEvent);

	if (event_add(&_acceptEvent, 0) == -1) {
		sockets::close(_acceptSocket.fd());
	}
}

// }}}
// {{{ void Acceptor::acceptHandler()

void Acceptor::acceptHandler(int fd) {
	if (fd != _acceptSocket.fd()) {
		LOG_ERROR << "Socket fd is invalid";
		return;		
	}
	InetAddress remoteAddr;
	int connfd = _acceptSocket.accept(&remoteAddr);
	LOG_DEBUG << "New connection: " << remoteAddr.toIpPort() << " fd:" << connfd;
	_newConnection(remoteAddr, connfd);
}

// }}}
}

