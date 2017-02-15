#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_ACCEPTOR_HPP_
#define ADBASE_NET_ACCEPTOR_HPP_

#include <adbase/Net/Types.hpp>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

class InetAddress;
class Socket;
namespace detail {
void acceptHandler(int, short, void *arg);
}
class Acceptor {
public:	
	typedef std::function<void (const InetAddress&, int)> NewConnectionCallback;
	Acceptor(EventBasePtr& base, const InetAddress& listenAddr, bool reuseport);
	void init();
	void acceptHandler(int fd);
	void setNewConnection(const NewConnectionCallback& cb) {
		_newConnection = cb;		
	}
	~Acceptor();

private:
	EventBasePtr& _base;
	Socket _acceptSocket;	
	typedef struct event Event;
	Event _acceptEvent;
	NewConnectionCallback _newConnection;
};

/*@}*/

}

#endif
