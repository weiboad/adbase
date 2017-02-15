//@IF @adserver
#ifndef @ADINF_PROJECT_NAME|upper@_HTTP_HPP_
#define @ADINF_PROJECT_NAME|upper@_HTTP_HPP_

#include "AdbaseConfig.hpp"
//@FOR @http_controllers
#include "Http/@REPLACE0@.hpp"
//@ENDFOR
#include "Http/Server.hpp"

// {{{ macros

#ifndef DECLARE_HTTP_CONTROLLER
#define DECLARE_HTTP_CONTROLLER(name) \
	    adserver::http:: name* _http##name = nullptr;
#endif
#ifndef ADSERVER_HTTP_ADD_CONTROLLER
#define ADSERVER_HTTP_ADD_CONTROLLER(name) do {\
    _http##name = new adserver::http:: name(_context);\
    _http##name->registerLocation(_http);\
} while(0)
#endif
#ifndef ADSERVER_HTTP_STOP
#define ADSERVER_HTTP_STOP(name) \
        delete _http##name;
#endif

// }}}
class Http {
public:
	Http(AdServerContext* context, adbase::http::Server* http);
	~Http();
	void addController();
	std::unordered_map<std::string, std::string> rewrite();
private:
	AdServerContext* _context;
	adbase::http::Server* _http;
	DECLARE_HTTP_CONTROLLER(Server);
	//@FOR @http_controllers
	DECLARE_HTTP_CONTROLLER(@REPLACE0@);
	//@ENDFOR

};

#endif
//@ENDIF
