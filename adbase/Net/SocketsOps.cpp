#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

#include <errno.h>
#include <fcntl.h>
#include <cstdio>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct sockaddr SA;
// {{{ void setNonBlockAndCloseOnExec()
#if VALGRIND || defined (NO_ACCEPT4) || !defined(SOCK_NONBLOCK)
void setNonBlockAndCloseOnExec(int sockfd) {
	// non-block
	int flags = ::fcntl(sockfd, F_GETFL, 0);	
	flags |= O_NONBLOCK;
	int ret = ::fcntl(sockfd, F_GETFL, flags);
	
	// close-on-exec
	flags = ::fcntl(sockfd, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	ret = ::fcntl(sockfd, F_SETFD, flags);

	(void)ret;
}
#endif
// }}}
namespace adbase {
#pragma GCC diagnostic ignored "-Wold-style-cast"
namespace sockets {
// {{{ void setNonBlock()
void setNonBlock(int sockfd) {
	int flags = ::fcntl(sockfd, F_GETFL, 0);	
	flags |= O_NONBLOCK;
	::fcntl(sockfd, F_GETFL, flags);
}

// }}}
// {{{ const struct sockaddr* sockaddrCast()

const struct sockaddr* sockaddrCast(const struct sockaddr_in* addr) {
	return static_cast<const struct sockaddr*>((const void*)addr);	
}

// }}}
// {{{ struct sockaddr* sockaddrCast()
 	
struct sockaddr* sockaddrCast(struct sockaddr_in* addr) {
	return static_cast<struct sockaddr*>((void*)addr);	
}

// }}}
// {{{ const struct sockaddr_in* sockaddrInCast()

const struct sockaddr_in* sockaddrInCast(const struct sockaddr* addr) {
	return static_cast<const struct sockaddr_in*>((const void*)addr);	
}

// }}}
// {{{ struct sockaddr_in* sockaddrInCast()

struct sockaddr_in* sockaddrInCast(struct sockaddr* addr) {
	return static_cast<struct sockaddr_in*>((void*)addr);	
}

// }}}
// {{{ int createNonblockingOrDie()

int createNonblockingOrDie() {
#if VALGRIND || !defined(SOCK_NONBLOCK)
	int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		LOG_SYSFATAL << "createNonblockingOrDie";		
	}

	::setNonBlockAndCloseOnExec(sockfd);
#else
	int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (sockfd < 0) {
		LOG_SYSFATAL << "createNonblockingOrDie";	
	}
#endif

	return sockfd;
}

// }}}
// {{{ void bindOrDie()

void bindOrDie(int sockfd, const struct sockaddr_in& addr) {
	int ret = ::bind(sockfd, sockaddrCast(&addr), static_cast<socklen_t>(sizeof addr));
	if (ret < 0) {
		LOG_SYSFATAL << "bindOrDie";	
	}	
}

// }}}
// {{{ void listenOrDie()

void listenOrDie(int sockfd) {
	int ret = ::listen(sockfd, SOMAXCONN);
	if (ret < 0) {
		LOG_SYSFATAL << "listenOrDie";	
	}	
}

// }}}
// {{{ int accept()

int accept(int sockfd, struct sockaddr_in* addr) {
	socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
#if VALGRIND || defined(NO_ACCEPT4) || !defined(SOCK_NONBLOCK)
	int connfd = ::accept(sockfd, sockaddrCast(addr), &addrlen);
	::setNonBlockAndCloseOnExec(connfd);
#else
	int connfd = ::accept4(sockfd, sockaddrCast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif	
	if (connfd < 0) {
		int savedErrno = errno;
		LOG_SYSERR << "accept";	
		switch (savedErrno) {
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:	
			case EPROTO:
			case EPERM:
			case EMFILE:
				errno = savedErrno;
				break;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				LOG_FATAL << "unexpected error of ::accept " << savedErrno;
				break;
			default:
				LOG_FATAL << "unknown error of ::accept " << savedErrno;
				break;
		}
	}

	return connfd;
}

// }}}
// {{{ int connect()

int connect(int sockfd, const struct sockaddr_in& addr) {
	return ::connect(sockfd, sockaddrCast(&addr), static_cast<socklen_t>(sizeof addr));	
}

// }}}
// {{{ void close()

void close(int sockfd) {
	if (::close(sockfd) < 0) {
		LOG_SYSERR << "close";	
	}	
}

// }}}
// {{{ void shutdownWrite()

void shutdownWrite(int sockfd) {
	if (::shutdown(sockfd, SHUT_WR) < 0) {
		LOG_SYSERR << "shutdownWrite";	
	} 	
}

// }}}
// {{{ void toIpPort()

void toIpPort(char* buf, size_t size,
					   const struct sockaddr_in& addr) {
	assert(size >= INET_ADDRSTRLEN);
	::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));	
	size_t end = ::strlen(buf);
	uint16_t port = networkToHost16(addr.sin_port);
	assert(size > end);
	snprintf(buf + end, size - end, ":%u", port);
}

// }}}
// {{{ void toIp()

void toIp(char* buf, size_t size,
				   const struct sockaddr_in& addr) {
	assert(size >= INET_ADDRSTRLEN);
	::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));	
}

// }}}
// {{{ void fromIpPort()

void fromIpPort(const char* ip, uint16_t port,
						 struct sockaddr_in* addr) {
	addr->sin_family = AF_INET;
	addr->sin_port = hostToNetwork16(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
		LOG_SYSERR << "fromIpPort";	
	}
}

// }}}
// {{{ int getSocketError()

int getSocketError(int sockfd) {
	int optval;
	socklen_t optlen = static_cast<socklen_t>(sizeof optval);
	
	if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
		return errno;	
	} else {
		return optval;	
	}
}

// }}}
// {{{ struct sockaddr_in getLocalAddr()

struct sockaddr_in getLocalAddr(int sockfd) {
	struct sockaddr_in localaddr;
	bzero(&localaddr, sizeof localaddr);
	socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
	if (::getsockname(sockfd, sockaddrCast(&localaddr), &addrlen) < 0) {
		LOG_SYSERR << "getLocalAddr";	
	}	

	return localaddr;
}

// }}}
// {{{ struct sockaddr_in getPeerAddr()

struct sockaddr_in getPeerAddr(int sockfd) {
	struct sockaddr_in peeraddr;
	bzero(&peeraddr, sizeof peeraddr);	
	socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
	if (::getpeername(sockfd, sockaddrCast(&peeraddr), &addrlen) < 0) {
		LOG_SYSERR << "getPeerAddr";	
	}

	return peeraddr;
}

// }}}
// {{{ bool isSelfConnect()

bool isSelfConnect(int sockfd) {
	struct sockaddr_in localaddr = getLocalAddr(sockfd);
	struct sockaddr_in peeraddr = getPeerAddr(sockfd);	
	return localaddr.sin_port == peeraddr.sin_port
		&& localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}

// }}}
}
#pragma GCC diagnostic error "-Wold-style-cast"
}
 	

