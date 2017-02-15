#include <adbase/Net.hpp>
#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

#include <netdb.h>
#include <strings.h>
#include <netinet/in.h>

namespace adbase {

// INADDR_ANY use (type)value casting.
#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Wold-style-cast"

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in), "InetAddress object len invalid");

// {{{ InetAddress::InetAddress()

InetAddress::InetAddress(uint16_t port, bool loopbackOnly) {
	bzero(&_addr, sizeof _addr);
	_addr.sin_family = AF_INET;
	in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
	_addr.sin_addr.s_addr = hostToNetwork32(ip);
	_addr.sin_port = hostToNetwork16(port);		
}

// }}}
// {{{ InetAddress::InetAddress()

InetAddress::InetAddress(std::string ip, uint16_t port) {
	bzero(&_addr, sizeof _addr);
	sockets::fromIpPort(ip.c_str(), port, &_addr);	
}

// }}}
// {{{ std::string InetAddress::toIpPort()

std::string InetAddress::toIpPort() const {
	char buf[32];
	sockets::toIpPort(buf, sizeof buf, _addr);	
	return buf;
}

// }}}
// {{{ std::string InetAddress::toIp()

std::string InetAddress::toIp() const {
	char buf[32];
	sockets::toIp(buf, sizeof buf, _addr);	
	return buf;
}

// }}}
// {{{ uint16_t InetAddress::toPort()

uint16_t InetAddress::toPort() const {
	return networkToHost16(_addr.sin_port);	
}

// }}}
// {{{ bool InetAddress::resolve()

#pragma GCC diagnostic ignored "-Wold-style-cast"
bool InetAddress::resolve(const std::string& hostname, std::string& ip) {
	struct addrinfo hints, *info;
    int rv;

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;  /* specify socktype to avoid dups */

    if ((rv = getaddrinfo(hostname.c_str(), nullptr, &hints, &info)) != 0) {
		LOG_SYSERR << gai_strerror(rv);
        return false;
    }

	char ipbuf[255];
    if (info->ai_family == AF_INET) {
        struct sockaddr_in *sa = (struct sockaddr_in *)info->ai_addr;
        inet_ntop(AF_INET, &(sa->sin_addr), ipbuf, 255);
    } else {
        struct sockaddr_in6 *sa = (struct sockaddr_in6 *)info->ai_addr;
        inet_ntop(AF_INET6, &(sa->sin6_addr), ipbuf, 255);
    }

    freeaddrinfo(info);
	ip = std::string(ipbuf);
    return true;
}

// }}}
// {{{ bool InetAddress::resolveAll()

bool InetAddress::resolveAll(const std::string& hostname, std::vector<std::string>& ips) {
	struct addrinfo hints, *info, *p;
    int rv;

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;  /* specify socktype to avoid dups */

    if ((rv = getaddrinfo(hostname.c_str(), nullptr, &hints, &info)) != 0) {
		LOG_SYSERR << gai_strerror(rv);
        return false;
    }

	for (p = info; p != nullptr; p = p->ai_next) {
		char ipbuf[255];
		memset(ipbuf, 0, sizeof(ipbuf));
		if (p->ai_family == AF_INET) {
			struct sockaddr_in *sa = (struct sockaddr_in *)p->ai_addr;
			inet_ntop(AF_INET, &(sa->sin_addr), ipbuf, 254);
		} else {
			struct sockaddr_in6 *sa = (struct sockaddr_in6 *)p->ai_addr;
			inet_ntop(AF_INET6, &(sa->sin6_addr), ipbuf, 254);
		}
		std::string ip = std::string(ipbuf);
		ips.push_back(ip);
	}

    freeaddrinfo(info);
    return true;
}
#pragma GCC diagnostic error "-Wold-style-cast"

// }}}
}
