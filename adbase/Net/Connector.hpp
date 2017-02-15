#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_CONNECTOR_HPP_
#define ADBASE_NET_CONNECTOR_HPP_

#include <adbase/Net/Types.hpp>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

namespace detail {
void handlerConnector(int sockfd, short evt, void *arg);
}
class InetAddress;
class Timer;
class Connector : public std::enable_shared_from_this<Connector> {
public:
	typedef std::function<void (int sockfd)> NewConnectionCallback;
	Connector(EventBasePtr& base, const InetAddress& serverAddr, Timer* timer);
	Connector(EventBasePtr& base, const std::string& hostname, int port, Timer* timer);
	~Connector();
	
	void setNewConnectionCallback(const NewConnectionCallback& cb) {
		_newConnectionCallback = cb;	
	}

	const InetAddress& serverAddress() const {
		return _serverAddr;	
	}

	void start(void* context = nullptr);
	void stop();
	void restart();
	void handleWrite(int sockfd);
	void handleError(int sockfd);

private:
	enum States { kDisconnected, kConnecting, kConnected };
	static const int kMaxRetryDelayMs = 30 * 1000;
	static const int kInitRetryDelayMs = 500;

	void setState(States s) {
		_state = s;	
	}
	void connect();
	void connecting(int sockfd);
	void retry(int sockfd);

	EventBasePtr& _base;
	Timer* _timer;
	std::string _hostname;
	int _port;
	InetAddress _serverAddr;
	bool _connect;
	States _state;
	NewConnectionCallback _newConnectionCallback;
	int _retryDelayMs;
	struct event _connectEvent;
	int _sockfd;
};

/*@}*/

}

#endif
