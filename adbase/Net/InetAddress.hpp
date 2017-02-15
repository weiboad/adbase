#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_INETADDRESS_HPP_
#define ADBASE_NET_INETADDRESS_HPP_

#include <string>
#include <vector>
#include <netinet/in.h>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

class InetAddress {
public:
	explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
	
	/// Constructs an endpoint with given ip and port.
	/// @c ip should be "1.2.3.4"
	InetAddress(std::string ip, uint16_t port);	

	/// Constructs an endpoint with given struct @c sockaddr_in
	/// Mostly used when accepting new connections
	InetAddress(const struct sockaddr_in& addr) :
		_addr(addr) {
	}

	std::string toIp() const;
	std::string toIpPort() const;
	uint16_t toPort() const;

	const struct sockaddr_in& getSockAddrInet() const {
		return _addr;	
	}

	void setSockAddrInet(const struct sockaddr_in& addr) {
		_addr = addr;
	}

	uint32_t ipNetEndian() const {
		return _addr.sin_addr.s_addr;	
	}

	uint16_t portNetEndian() const {
		return _addr.sin_port;	
	}

	/**
	 *  resolve hostname to IP address, not changing port or sin_family
	 *  return true on success.
	 *  thread safe
	 */
	static bool resolve(const std::string& hostname, std::string& ip);
	static bool resolveAll(const std::string& hostname, std::vector<std::string>& ip);

private:
	struct sockaddr_in _addr;
};

/*@}*/
		
}

#endif
