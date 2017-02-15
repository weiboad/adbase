#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_SOCKETSOPS_HPP_
#define ADBASE_NET_SOCKETSOPS_HPP_

#include <arpa/inet.h>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

namespace sockets {
int createNonblockingOrDie();
void setNonBlock(int sockfd);
int connect(int sockfd, const struct sockaddr_in& addr);
void bindOrDie(int sockfd, const struct sockaddr_in& addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in* addr);
void close(int sockfd);
void shutdownWrite(int sockfd);

void toIpPort(char* buf, size_t size, const struct sockaddr_in& addr);
void toIp(char* buf, size_t size, const struct sockaddr_in& addr);
void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);

int getSocketError(int sockfd);

const struct sockaddr* sockaddrCast(const struct sockaddr_in* addr);
struct sockaddr* sockaddrCast(struct sockaddr_in* addr);
const struct sockaddr_in* sockaddrInCast(const struct sockaddr* addr);
struct sockaddr_in* sockaddrInCast(struct sockaddr* addr);

struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);
bool isSelfConnect(int sockfd);
}	

/*@}*/
}
#endif
