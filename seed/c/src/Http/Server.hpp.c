//@IF @adserver
#ifndef SEEDTEST_HTTP_SERVER_HPP_
#define SEEDTEST_HTTP_SERVER_HPP_

#include "HttpInterface.hpp"

namespace adserver {
namespace http {
class Server : HttpInterface {
public:
	Server(AdServerContext* context);
	void registerLocation(adbase::http::Server* http);
	void status(adbase::http::Request* request, adbase::http::Response* response, void*);
    void metrics(adbase::http::Request* request, adbase::http::Response* response, void*);

private:
    const std::string formatMetric(std::string key, uint64_t value, std::unordered_map<std::string, std::string> tags);
    const std::string getKey(const std::string& moduleName, const std::string& metricName);
    uint64_t toUint64(std::string value);
    uint64_t toUint64(double value);
};
}
}

#endif
//@ENDIF
