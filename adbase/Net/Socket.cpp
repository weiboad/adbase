#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>
#include <cstdio>
#include <linux/version.h>

namespace adbase {
// {{{ Socket::~Socket()

Socket::~Socket() {
	LOG_DEBUG << "Close fd " << _sockfd;
	if (_isFreeClose) {
		sockets::close(_sockfd);	
	}
}

// }}}
// {{{ bool Socket::getTcpInfo()

bool Socket::getTcpInfo(struct tcp_info* tcpi) const {
	socklen_t len = sizeof(*tcpi);
	bzero(tcpi, len);
	return ::getsockopt(_sockfd, SOL_TCP, TCP_INFO, tcpi, &len) == 0;	
}

// }}}
// {{{ bool Socket::getTcpInfoString()

bool Socket::getTcpInfoString(char* buf, int len) const {
	struct tcp_info tcpi;
	bool ok = getTcpInfo(&tcpi);
	if (ok)	{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 32))
		snprintf(buf, len, "unrecovered=%u "
				 "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
				 "lost=%u retrans=%u rtt=%u rttvar=%u "
				 "sshthresh=%u cwnd=%u",
				 tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
				 tcpi.tcpi_rto,          // Retransmit timeout in usec
				 tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
				 tcpi.tcpi_snd_mss,
				 tcpi.tcpi_rcv_mss,
				 tcpi.tcpi_lost,         // Lost packets
				 tcpi.tcpi_retrans,      // Retransmitted packets out
				 tcpi.tcpi_rtt,          // Smoothed round trip time in usec
				 tcpi.tcpi_rttvar,       // Medium deviation
				 tcpi.tcpi_snd_ssthresh,
				 tcpi.tcpi_snd_cwnd); 
#else
		snprintf(buf, len, "unrecovered=%u "
				 "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
				 "lost=%u retrans=%u rtt=%u rttvar=%u "
				 "sshthresh=%u cwnd=%u total_retrans=%u",
				 tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
				 tcpi.tcpi_rto,          // Retransmit timeout in usec
				 tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
				 tcpi.tcpi_snd_mss,
				 tcpi.tcpi_rcv_mss,
				 tcpi.tcpi_lost,         // Lost packets
				 tcpi.tcpi_retrans,      // Retransmitted packets out
				 tcpi.tcpi_rtt,          // Smoothed round trip time in usec
				 tcpi.tcpi_rttvar,       // Medium deviation
				 tcpi.tcpi_snd_ssthresh,
				 tcpi.tcpi_snd_cwnd,
				 tcpi.tcpi_total_retrans);  // Total retransmits for entire connection 

#endif
	}

	return ok;
}

// }}}
// {{{ void Socket::bindAddress()

void Socket::bindAddress(const InetAddress& addr) {
	sockets::bindOrDie(_sockfd, addr.getSockAddrInet());	
}

// }}}
// {{{ void Socket::listen()

void Socket::listen() {
	sockets::listenOrDie(_sockfd);	
}

// }}}
// {{{ int Socket::accept()

int Socket::accept(InetAddress* peeraddr) {
	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	int connfd = sockets::accept(_sockfd, &addr);	
	if (connfd >= 0) {
		peeraddr->setSockAddrInet(addr);	
	}
	return connfd;
}

// }}}
// {{{ void Socket::shutdownWrite()

void Socket::shutdownWrite() {
	sockets::shutdownWrite(_sockfd);	
}

// }}}
// {{{ void Socket::setTcpNoDelay()

void Socket::setTcpNoDelay(bool on) {
	int optval = on ? 1 : 0;
	::setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY,
				 &optval, static_cast<socklen_t>(sizeof optval));	
}

// }}}
// {{{ void Socket::setReuseAddr()

void Socket::setReuseAddr(bool on) {
	int optval = on ? 1 : 0;
	::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR,
				 &optval, static_cast<socklen_t>(sizeof optval));	
}

// }}}
// {{{ void Socket::setReusePort()

void Socket::setReusePort(bool on) {
#ifdef SO_REUSEPORT
	int optval = on ? 1 : 0;
	int ret = ::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT,
						   &optval, static_cast<socklen_t>(sizeof optval));
	if (ret < 0 && on) {
		LOG_SYSERR << "SO_REUSEPORT failed.";	
	}	
#else
	if (on) {
		LOG_ERROR << "SO_REUSEPORT is not supported.";	
	}
#endif	
}

// }}}
// {{{ void Socket::setKeepAlive()

void Socket::setKeepAlive(bool on) {
	int optval = on ? 1 : 0;
	::setsockopt(_sockfd, SOL_SOCKET, SO_KEEPALIVE,
				 &optval, static_cast<socklen_t>(sizeof optval));	
}

// }}}
}
