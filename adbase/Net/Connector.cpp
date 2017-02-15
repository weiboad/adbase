#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace detail {
// {{{ void handlerConnector()

void handlerConnector(int sockfd, short evt, void *arg) {
	Connector* conn = reinterpret_cast<Connector*>(arg);
	if (evt & EV_WRITE || evt & EV_READ) {		
		conn->handleWrite(sockfd);		
	} else {
		conn->handleError(sockfd);		
	}
}

// }}}
}
const int Connector::kMaxRetryDelayMs;
// {{{ Connector::Connector()

Connector::Connector(EventBasePtr& base, const std::string& hostname, int port, Timer* timer) :
	_base(base),
	_timer(timer),
	_hostname(hostname),
	_port(port),
	_connect(false),
	_state(kDisconnected),
	_retryDelayMs(kInitRetryDelayMs) {
	LOG_TRACE << "ctor[" << this << "]";
}

// }}}
// {{{ Connector::Connector()

Connector::Connector(EventBasePtr& base, const InetAddress& serverAddr, Timer* timer) :
	_base(base),
	_timer(timer),
	_serverAddr(serverAddr),
	_connect(false),
	_state(kDisconnected),
	_retryDelayMs(kInitRetryDelayMs) {
	LOG_TRACE << "ctor[" << this << "]";
	_hostname = serverAddr.toIp();
	_port = serverAddr.toPort();
}

// }}}
// {{{ Connector::~Connector()

Connector::~Connector() {
	LOG_INFO << "dtor[" << this << "]";	
	stop();
}

// }}}
// {{{ void Connector::start()

void Connector::start(void*) {
	_connect = true;
	connect();
}

// }}}
// {{{ void Connector::stop()

void Connector::stop() {
	_connect = false;
	if (_state == kConnecting) {
		retry(_sockfd);
	}
}

// }}}
// {{{ void Connector::restart()

void Connector::restart() {
	stop();
	_retryDelayMs = kInitRetryDelayMs;
	start();
}

// }}}
// {{{ void Connector::connect()

void Connector::connect() {
	int sockfd = sockets::createNonblockingOrDie();
	_sockfd = sockfd;
	if (!_hostname.empty()) {
		std::string ip;
		bool ret = InetAddress::resolve(_hostname, ip);
		if (!ret) {
			retry(sockfd);	
		}
		_serverAddr = InetAddress(ip, static_cast<uint16_t>(_port));
		// 日志记录准确的ip地址
		_hostname = ip;
	}

	int ret = sockets::connect(sockfd, _serverAddr.getSockAddrInet());
	int savedErrno = (ret == 0) ? 0 :  errno;
	switch (savedErrno) {
		case 0:
		case EINPROGRESS:
		case EINTR:
		case EISCONN:
			connecting(sockfd);	
			break;

		 case EAGAIN:
		 case EADDRINUSE:
		 case EADDRNOTAVAIL:
		 case ECONNREFUSED:
		 case ENETUNREACH:
			retry(sockfd);
			break;

		 case EACCES:
		 case EPERM:
		 case EAFNOSUPPORT:
		 case EALREADY:
		 case EBADF:
		 case EFAULT:
		 case ENOTSOCK:
			LOG_SYSERR << "connect error in Connector::connect " << savedErrno;
			sockets::close(sockfd);
			break;

		 default:
			LOG_SYSERR << "Unexpected error in Connector::connect " << savedErrno;
			sockets::close(sockfd);
			break;
	}
}
 
// }}}
// {{{ void Connector::connecting()

void Connector::connecting(int sockfd) {
	setState(kConnecting);
	event_set(&_connectEvent, sockfd, EV_WRITE, detail::handlerConnector, this);
	event_base_set(_base.get(), &_connectEvent);
	if (event_add(&_connectEvent, 0) == -1) {
		LOG_SYSERR << "Set connector event fail.";
	}
	LOG_TRACE << "Connector connect event start.";
}

// }}}
// {{{ void Connector::handleWrite()

void Connector::handleWrite(int sockfd) {
	LOG_TRACE << "Connector::handlerWrite " << _state;	
	if (_state == kConnecting) {
		int err = sockets::getSocketError(sockfd);
		if (err) {
			LOG_WARN << "Connector::handlerWrite - SO_ERROR = " << err << " " 
					 << " " << _hostname << ":" << _port << " "
					 << adbase::strerror_tl(err);	
			retry(sockfd);
		} else if (sockets::isSelfConnect(sockfd)) {
			LOG_WARN << "Connector::handlerWrite - Self connect";	
			retry(sockfd);
		} else {
			setState(kConnected);	
			if (_connect) {
				_newConnectionCallback(sockfd);
			} else {
				sockets::close(sockfd);	
			}
		}
	} else {
		// what happened?	
	}
}

// }}}
// {{{ void Connector::handleError()

void Connector::handleError(int sockfd) {
	LOG_ERROR << "Connector::handleError state=" << _state;
	if (_state == kConnecting) {
		int err = sockets::getSocketError(sockfd);
		LOG_TRACE << "SO_ERROR = " << err << " " << adbase::strerror_tl(err);
		retry(sockfd);
	}
}

// }}}
// {{{ void Connector::retry()

void Connector::retry(int sockfd) {
  sockets::close(sockfd);
  setState(kDisconnected);
  if (_connect) {
    LOG_INFO << "Connector::retry - Retry connecting to " << _serverAddr.toIpPort()
             << " in " << _retryDelayMs << " milliseconds. ";
	_timer->runAfter(_retryDelayMs, std::bind(&Connector::start, shared_from_this(), std::placeholders::_1), nullptr);
    _retryDelayMs = std::min(_retryDelayMs * 2, kMaxRetryDelayMs);
  } else {
    LOG_DEBUG << "do not connect";
  }
}

// }}}
}

