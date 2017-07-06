#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>
#include <climits>

namespace adbase {
namespace detail {
// {{{ void pipeHandler()

void pipeHandler(int, short evt, void *arg) {
	if (evt & EV_READ) {
		LOG_DEBUG << "Pipe read event.";
		TcpWorker* worker = reinterpret_cast<TcpWorker*>(arg);
		worker->pipeHandler();
	} else {
		LOG_ERROR << "Pipe event error.";	
	}
}

// }}}
}
// {{{ TcpWorker::TcpWorker()

TcpWorker::TcpWorker(int pipe, Queue<ConnectionBaseInfo>* queue, int threadId, std::string& serverName) :
	_pipe(pipe),
	_queue(queue),
	_threadId(threadId),
	_name(serverName),
	_connId(0) {
	EventBasePtr tmp(event_base_new(), std::ptr_fun(&event_base_free));
	_base = std::move(tmp);
}

// }}}
// {{{ TcpWorker::~TcpWorker()

TcpWorker::~TcpWorker() {
    LOG_ERROR << "Tcp Worker stop.";	
}

// }}}
// {{{ void TcpWorker::start()

void TcpWorker::start() {
	event_set(&_pipeEvent, _pipe, EV_READ | EV_PERSIST, detail::pipeHandler, this);
	event_base_set(_base.get(), &_pipeEvent);

	if (event_add(&_pipeEvent, 0) == -1) {
		LOG_FATAL << "Set pipe event fail.";
	}
	LOG_TRACE << "Worker start.";

	event_base_loop(_base.get(), 0);
}

// }}} 
// {{{ void TcpWorker::pipeHandler()

void TcpWorker::pipeHandler() {
	char buf[1];
	if (read(_pipe, buf, 1) != 1) {
		LOG_ERROR << "Read connection pipe fail.";	
		return;
	}

	if (buf[0] == 'c') {
		ConnectionBaseInfo baseInfo;
		bool ret = _queue->tryPop(baseInfo);	
		if (!ret) {
			LOG_ERROR << "POP Connection queue fail.";	
			return;
		}
		LOG_TRACE << "Connection fd:" << baseInfo.connfd;
		if (_connId > INT_MAX) {
			_connId = 0;	
		}
		_connId++;
		TcpConnectionPtr connection(new TcpConnection(_base, baseInfo.connfd,
								  baseInfo.localAddress, baseInfo.peerAddress, 
								  _connId,
								  std::bind(&TcpWorker::removeConnection, this, std::placeholders::_1)));
		connection->init();
		connection->setMessageCallback(_messageCallback);
		connection->setWriteCompleteCallback(_writeCompleteCallback);
		connection->setConnectionCookiePrefix(
			std::bind(&TcpWorker::connectionCookiePrefix, this)
		);
		_connectionCallback(connection);
		_connections[_connId] = std::move(connection);
	}

	if (buf[0] == 's') { // stop
		if (!event_base_got_break(_base.get())) {
			event_base_loopbreak(_base.get());
			LOG_ERROR << "Stop tcp work event loop.";
		}
	}
}

// }}}
// {{{ void TcpWorker::removeConnection()

void TcpWorker::removeConnection(int connId) {
	LOG_DEBUG << "Remove connection " << connId;
	if (_closeCallback) {
		_closeCallback(_connections[connId]);	
	}
	_connections.erase(connId);
}

// }}}
// {{{ std::string TcpWorker::connectionCookiePrefix()

std::string TcpWorker::connectionCookiePrefix() {
	return getServerName() + "-" + std::to_string(getWorkerId());	
}

// }}}
}
