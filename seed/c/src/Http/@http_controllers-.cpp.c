//@IF @adserver
#include "@FILEREPLACE0@.hpp"

namespace adserver {
namespace http {
// {{{ @FILEREPLACE0@::@FILEREPLACE0@()

@FILEREPLACE0@::@FILEREPLACE0@(AdServerContext* context) :
	HttpInterface(context) {
}

// }}}
// {{{ void @FILEREPLACE0@::registerLocation()

void @FILEREPLACE0@::registerLocation(adbase::http::Server* http) {
	ADSERVER_HTTP_ADD_API(http, @FILEREPLACE0@, index)
}

// }}}
// {{{ void @FILEREPLACE0@::index()

void @FILEREPLACE0@::index(adbase::http::Request* request, adbase::http::Response* response, void*) {
	(void)request;
	responseJson(response, "{\"msg\": \"hello adinf\"}", 0, "");
}

// }}}
}
}
//@ENDIF
