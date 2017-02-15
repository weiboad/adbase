#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_SOCKET_HPP_
#define ADBASE_NET_SOCKET_HPP_

struct tcp_info;
namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

class InetAddress;	
class Socket {
public:
	explicit Socket(int sockfd, bool isFreeClose = true) : 
		_sockfd(sockfd),
		_isFreeClose(isFreeClose) {
	}	

	~Socket();
	
	int fd() const {
		return _sockfd;	
	}		

	bool getTcpInfo(struct tcp_info*) const;
	bool getTcpInfoString(char* buf, int len) const;

	/// abort if address in use
	void bindAddress(const InetAddress& localaddr);

	/// abort if address in use
	void listen();

	int accept(InetAddress* peeraddr);

	void shutdownWrite();

	///  Enable/disable TCP_NODELAY (disable/enable Nagle's algorithm).
	void setTcpNoDelay(bool on);

	/// Enable/disable SO_REUSEADDR
	void setReuseAddr(bool on);

	/// Enable/disable SO_REUSEPORT
	void setReusePort(bool on);

	/// Enable/disable SO_KEEPALIVE
	void setKeepAlive(bool on);

private:
	const int _sockfd;
	bool _isFreeClose;
};

/*@}*/

}

#endif
