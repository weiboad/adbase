#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main() {
	/// url encode/decode example
	std::string url = "http://www.weibo.com?test=1&test??????"; 
	std::string encode = adbase::urlEncode(url);
	LOG_INFO << encode;
	LOG_INFO << adbase::urlDecode(encode);
    return 0;
}
