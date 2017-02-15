//@IF @adserver
#include "HttpInterface.hpp"
#include "App.hpp"

namespace adserver {
namespace http {
// {{{ HttpInterface::HttpInterface()

HttpInterface::HttpInterface(AdServerContext* context) :
	_context(context) {
}

// }}}
// {{{ HttpInterface::~HttpInterface()

HttpInterface::~HttpInterface() {
}

// }}}
// {{{ void HttpInterface::registerLocation()

void HttpInterface::registerLocation(adbase::http::Server* http) {
	(void)http;
}

// }}}
// {{{ void HttpInterface::responseHeader()

void HttpInterface::responseHeader(adbase::http::Response* response) {
	response->addHeader("Content-Type", "application/json; charset=utf-8");
	response->setHeader("Server", _context->config->httpServerName, true);
	response->setHeader("Connection", "keep-alive");
}

// }}}
// {{{ void HttpInterface::responseJson()

void HttpInterface::responseJson(adbase::http::Response* response, const std::string& data, uint32_t ret, const std::string& msg, bool isRawString) {
	std::string result = "{\"code\":" + std::to_string(ret) + ",";
	result += "\"baseid\":" + std::to_string(_context->app->getSeqId()) + ",";
	std::string rawData = data;
	if (isRawString) {
		rawData = "\"" + rawData + "\"";
	}
	result += "\"data\":" + rawData + ",";
	std::string rawMsg = "\"" + msg + "\"}";
	result += "\"msg\":" + rawMsg;
	responseHeader(response);
	response->setContent(result);
	response->sendReply();
}

// }}}
}
}
//@ENDIF
