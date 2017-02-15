#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_TYPES_HPP_
#define ADBASE_NET_TYPES_HPP_

#include <event.h>
#include <functional>  
#include <thread>  

namespace adbase {

/// libevent
typedef std::shared_ptr<event_base> EventBasePtr;
typedef std::shared_ptr<bufferevent> BufferEventPtr;
typedef std::shared_ptr<event> EventPtr;

class InetAddress; 
typedef struct ConnectionBaseInfo {
	int connfd;
	InetAddress localAddress;		
	InetAddress peerAddress;		
} ConnectionBaseInfo;


class TcpConnection;
class Timestamp;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback; 
typedef std::function<void (int)> RemoveConnectionCallback; 
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback; 
typedef std::function<std::string ()> ConnectionCookiePrefix; 
typedef std::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback; 
typedef std::function<void (const TcpConnectionPtr&,
							evbuffer*,
							Timestamp)> MessageCallback;
namespace detail {
void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn,
							evbuffer* buffer,
							Timestamp receiveTime);
}

// timer
typedef std::function<void (uint64_t)> DelTimerCallback;
typedef std::function<void (void*)> TimerCallback;
}

#endif
