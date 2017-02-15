//@IF @adserver
#ifndef @ADINF_PROJECT_NAME|upper@_HTTP_@FILEREPLACE0|upper@_HPP_
#define @ADINF_PROJECT_NAME|upper@_HTTP_@FILEREPLACE0|upper@_HPP_

#include "HttpInterface.hpp"

namespace adserver {
namespace http {
class @FILEREPLACE0@ : HttpInterface {
public:
	@FILEREPLACE0@(AdServerContext* context);
	void registerLocation(adbase::http::Server* http);
	void index(adbase::http::Request* request, adbase::http::Response* response, void*);
};
}
}

#endif
//@ENDIF
