//@IF @adserver
#include "Http.hpp"

// {{{ Http::Http()

Http::Http(AdServerContext* context, adbase::http::Server* http) :
	_context(context),
	_http(http) {
}

// }}}
// {{{ Http::~Http()

Http::~Http() {
	ADSERVER_HTTP_STOP(Server);
	//@FOR @http_controllers
	ADSERVER_HTTP_STOP(@REPLACE0@);
	//@ENDFOR
}

// }}}
// {{{ void Http::addController()

void Http::addController() {
	ADSERVER_HTTP_ADD_CONTROLLER(Server);
    //@FOR @http_controllers
	ADSERVER_HTTP_ADD_CONTROLLER(@REPLACE0@);
    //@ENDFOR
}

// }}}
// {{{ std::unordered_map<std::string, std::string> Http::rewrite()

std::unordered_map<std::string, std::string> Http::rewrite() {
	std::unordered_map<std::string, std::string> urls;
	return urls;
}

// }}}
//@ENDIF
