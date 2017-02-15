#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
// {{{ TcpClient::TcpClient()

TcpClient::TcpClient(EventBasePtr& base, const InetAddress& serverAddr, const std::string& name, Timer* timer) :
	_base(base),
	_connector(new Connector(base, serverAddr, timer)),
	_name(name),
	_timer(timer),
	_connectionCallback(detail::defaultConnectionCallback),
	_messageCallback(detail::defaultMessageCallback),
	_retry(false),
	_connect(true),
	_nextConnId(1) {
	_connector->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
	LOG_INFO << "TcpClient::TcpClient[" << _name << "] - connector " << _connector.get();		
}

// }}}
// {{{ TcpClient::TcpClient()

TcpClient::TcpClient(EventBasePtr& base, const std::string& hostname, int port, const std::string& name, Timer* timer) :
	_base(base),
	_connector(new Connector(base, hostname, port, timer)),
	_name(name),
	_timer(timer),
	_connectionCallback(detail::defaultConnectionCallback),
	_messageCallback(detail::defaultMessageCallback),
	_retry(false),
	_connect(true),
	_nextConnId(1) {
	_connector->setNewConnectionCallback(std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
	LOG_INFO << "TcpClient::TcpClient[" << _name << "] - connector " << _connector.get();		
}

// }}}
// {{{ TcpClient::~TcpClient()

TcpClient::~TcpClient() {
	LOG_INFO << "TcpClient::~TcpClient[" << _name
			 << "] - connector " << _connector.get();	
}

// }}}
// {{{ void TcpClient::connect()

void TcpClient::connect() {
	_connect = true;
	_connector->start();
	LOG_INFO << "TcpClient::connect[" << _name << "] - connecting to "	
			 << _connector->serverAddress().toIpPort();
}

// }}}
// {{{ void TcpClient::disconnect()

void TcpClient::disconnect() {
	_connect = false;	
	{
		std::lock_guard<std::mutex> lk(_mut);
		if (_connection) {
			_connection->shutdown();	
		}
	}
}

// }}}
// {{{ void TcpClient::stop()

void TcpClient::stop() {
	_connect = false;
	_connector->stop();	
}

// }}}
// {{{ void TcpClient::newConnection()

void TcpClient::newConnection(int sockfd) {
	InetAddress peerAddr(sockets::getPeerAddr(sockfd));	
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	TcpConnectionPtr conn(
		new TcpConnection(_base, sockfd, localAddr,
			peerAddr, _nextConnId,
			std::bind(&TcpClient::removeConnection, this, std::placeholders::_1)
		)
	);
	conn->setMessageCallback(_messageCallback);
	conn->setWriteCompleteCallback(_writeCompleteCallback);
	conn->init();
	{
		std::lock_guard<std::mutex> lk(_mut);
		_connection = conn;
	}
	_connectionCallback(_connection);
}

// }}}
// {{{ void TcpClient::removeConnection()

void TcpClient::removeConnection(int) {
	{
		std::lock_guard<std::mutex> lk(_mut);
		_connection.reset();
	}
	
	if (_retry && _connect) {
		LOG_INFO << "TcpClient::connect[" << _name << "] - Reconnecting to "
				 << _connector->serverAddress().toIpPort();	
		_connector->restart();
	}
}

// }}}
}

