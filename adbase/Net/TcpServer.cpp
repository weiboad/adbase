#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>
#include <unistd.h>

namespace adbase {
// {{{ TcpServer::TcpServer()

TcpServer::TcpServer(EventBasePtr& base, InetAddress& listenAddr, const std::string& name) :
	_base(base),
	_listenAddr(listenAddr),
	_name(name),
	_acceptor(base, listenAddr, false),
	_connectionCallback(&detail::defaultConnectionCallback),
	_messageCallback(&detail::defaultMessageCallback) {
	_acceptor.setNewConnection(std::bind(&TcpServer::newConnection, this, 
							   std::placeholders::_1, std::placeholders::_2));
}

// }}}
// {{{ void TcpServer::start()

void TcpServer::start(int threadNum) {
	_threadNum = threadNum;
		
	// 初始化管道信息 及 worker 线程
	for (int i = 0; i < threadNum; i++) {
		int fds[2];
		if (pipe(fds)) {
			LOG_FATAL << "Can't create notify pipe";	
		}
		std::vector<int> pipefds;
		pipefds.push_back(fds[0]);
		pipefds.push_back(fds[1]);
		_pipeMap[i] = pipefds;
		ThreadPtr Thread(new std::thread(std::bind(&TcpServer::threadFunc, this, std::placeholders::_1), i), &TcpServer::deleteThread);
		LOG_DEBUG << "Create worker thread success";
		Threads.push_back(std::move(Thread));
	}

	// 初始化 acceptor
	_acceptor.init();
}

// }}}
// {{{ void TcpServer::stop()

void TcpServer::stop() {
	for (auto &t : _pipeMap) {
		auto pipefds = t.second;	
		char buf[1];
		buf[0] = 's';
		if (write(pipefds[1], buf, 1) != 1) {
			LOG_ERROR << "Writing to thread stop pipe fail.";
		}
		LOG_ERROR << "notify tcpserver work thread stop.";
	}
}

// }}}
// {{{ void TcpServer::threadFunc()

void TcpServer::threadFunc(int id) {
	TcpWorker worker(_pipeMap[id][0], &_connections[id], id, _name);
	worker.setConnectionCallback(_connectionCallback);
	worker.setMessageCallback(_messageCallback);
	worker.setWriteCompleteCallback(_writeCompleteCallback);
	worker.setCloseCallback(_closeCallback);
	worker.start();	
}

// }}}
// {{{ void TcpServer::newConnection()

void TcpServer::newConnection(const InetAddress& remoteAddr, int connfd) {
	int workerId = remoteAddr.toPort() % _threadNum;
	auto pipefds = _pipeMap[workerId];

	// 一定要先入队列后通知
	ConnectionBaseInfo baseInfo;
	baseInfo.connfd = connfd;
	baseInfo.peerAddress = remoteAddr;
	baseInfo.localAddress = _listenAddr;
	_connections[workerId].push(baseInfo);

	char buf[1];
	buf[0] = 'c';
	if (write(pipefds[1], buf, 1) != 1) {
		LOG_ERROR << "Writing to thread notify pipe";	
	}
}

// }}}
// {{{ TcpServer::deleteThread()

void TcpServer::deleteThread(std::thread *t) {
	t->join();
	delete t;
	LOG_ERROR << "Tcp server main thread delete.";
}

// }}}
// {{{ TcpServer::~TcpServer()

TcpServer::~TcpServer() {
	
}

// }}}
}

