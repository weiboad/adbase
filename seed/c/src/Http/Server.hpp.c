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
};
}
}

#endif
//@ENDIF
