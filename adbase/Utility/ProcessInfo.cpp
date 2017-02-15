#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Version.hpp>
#include <sys/stat.h>
#include <dirent.h>

       #include <linux/if_packet.h>
       #include <net/ethernet.h> /* the L2 protocols */

	
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
       #include <sys/ioctl.h>

namespace adbase {
// {{{ pid_t pid()

pid_t pid() {
	return ::getpid();	
}

// }}}
// {{{ std::string pidString()

std::string pidString() {
	char buf[32];
	snprintf(buf, sizeof buf, "%d", pid());	
	return buf;
}

// }}}
// {{{ std::string hostname()

std::string hostname() {
	char buf[256];
	if (::gethostname(buf, sizeof buf) == 0) {
		buf[sizeof(buf) - 1] = '\0';	
		return buf;
	} else {
		return "unknownhost";	
	}
}

// }}}
// {{{ std::unordered_map<std::string, std::unordered_map<std::string, std::string>> ifconfig()

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> ifconfig() {
	std::unordered_map<std::string, std::string> ips;
	std::unordered_map<std::string, std::string> macs;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> result;

    int i=0;
    int sockfd;
    char buf[512];
    struct ifreq *ifreq;

	//初始化ifconf
    struct ifconf ifconf;
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return result;
    }

    ioctl(sockfd, SIOCGIFCONF, &ifconf);    //获取所有接口信息
    //接下来一个一个的获取IP地址
    ifreq = reinterpret_cast<struct ifreq*>(buf);
    for(i = (ifconf.ifc_len / static_cast<int>(sizeof(struct ifreq))); i > 0; i--) {
		std::unordered_map<std::string, std::string> item;
		item["ip"] = inet_ntoa(reinterpret_cast<struct sockaddr_in*>(&(ifreq->ifr_addr))->sin_addr);
		if (ioctl(sockfd, SIOCGIFHWADDR, ifreq) == 0) {
			unsigned char* mac = reinterpret_cast<unsigned char*>(ifreq->ifr_hwaddr.sa_data);
			char macstr[18];
			sprintf(macstr, "%02x:%02x:%02x:%02x:%02x:%02x", 
							static_cast<unsigned char>(mac[0]),
							static_cast<unsigned char>(mac[1]),
							static_cast<unsigned char>(mac[2]),
							static_cast<unsigned char>(mac[3]),
							static_cast<unsigned char>(mac[4]),
							static_cast<unsigned char>(mac[5]));
			macstr[17] = '\0';
			item["mac"] = macstr;
		}
		result[ifreq->ifr_name] = item;
        ifreq++;
	}
		
	close(sockfd);
	return result;
}

// }}}
// {{{ std::string procname()

std::string procname() {
	return procname(procStat());
}

// }}}
// {{{ std::string procname()

std::string procname(const std::string& stat) {
	std::string name;
	size_t lp = stat.find('(');
	size_t rp = stat.find(')');
	if (lp != std::string::npos && rp != std::string::npos && lp < rp) {
		name.append(stat.c_str() + lp + 1, static_cast<int>(rp - lp - 1));
	}
	return name;	
}

// }}}
// {{{ std::string procStatus()

std::string procStatus() {
	std::string result;
	readFile("/proc/self/status", 65536, &result);
	return result;
}

// }}}
// {{{ std::string procStat()

std::string procStat() {
	std::string result;
	readFile("/proc/self/stat", 65536, &result);
	return result;
}

// }}}
// {{{ const std::unordered_map<std::string, std::string> procStats()

const std::unordered_map<std::string, std::string> procStats()
{
    int pageSize = static_cast<int>(sysconf(_SC_PAGESIZE));
    std::unordered_map<std::string, std::string> stat;
    std::vector<std::string> stats = adbase::explode(procStat(), ' ', true);
    if (stats.size() < 24) {
        return stat;
    }

    // process name
    stat["name"] = stats.at(1);

    // State
    std::unordered_map<std::string, std::string> stateMap({{"R", "1"}, {"S", "2"},
        {"D", "3"}, {"T", "4"}, {"Z", "5"}, {"X", "6"}});
    if (stateMap.find(stats.at(2)) == stateMap.end()) {
        stat["state"] = "7";
    } else {
        stat["state"] = stateMap.at(stats.at(2));
    }

    // thread num 19
    stat["thread_num"] = stats.at(19);

    // rss 23
    uint64_t rss = std::stol(stats.at(23));
    rss = rss * pageSize;
    stat["rss"] = std::to_string(rss);

    // vsize 22
    stat["vsize"] = stats.at(22);

    // fd num
    stat["fds"] = std::to_string(procFdNum());

	// adbase version
	stat["adbase"]    = ADBASE_VERSION;
	stat["adbase_so"] = ADBASE_SOVERSION;

    return stat;
}

// }}}
// {{{ int procFdNum()

int procFdNum() {
	std::string fdDir = "/proc/self/fd";
	std::vector<std::string> excludes;
	std::vector<std::string> pathInfo;
	recursiveDir(fdDir, false, excludes, pathInfo);
    return static_cast<int>(pathInfo.size());
}

// }}}
}
