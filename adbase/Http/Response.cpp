#include <adbase/Logging.hpp>
#include <adbase/Http.hpp>
#include <evhttp.h>

namespace adbase {
namespace http {
// {{{ Response::Response()

Response::Response(evhttp_request *req) : 
	_request(req), 
	_buffer(evbuffer_new(), &evbuffer_free) {
}

// }}}
// {{{ int Response::setHeader()

int Response::setHeader(const std::string &key, const std::string &value, bool isReplace) {
	struct evkeyvalq* evheader = evhttp_request_get_output_headers(_request);
	const char *headerValue = evhttp_find_header(evheader, key.c_str());
	if (headerValue != nullptr) {		
		if (isReplace) {
			evhttp_remove_header(evheader, key.c_str());
		} else {
			return 0;	
		}
	}

	return evhttp_add_header(evheader, key.c_str(), value.c_str());
}

// }}}
// {{{ int Response::addHeader()

int Response::addHeader(const std::string &key, const std::string &value) {
	struct evkeyvalq* evheader = evhttp_request_get_output_headers(_request);
	return evhttp_add_header(evheader, key.c_str(), value.c_str());
}

// }}}
// {{{ void Response::setContent()
	
void Response::setContent(const std::string &data) {
	if (!_content.empty()) {
		_content.clear();	
	}

	_content.push_back(data);
}

// }}}
// {{{ void Response::appendContent()

void Response::appendContent(const std::string &data, bool isAfter) {
	if (isAfter) {
		_content.push_back(data);	
	} else {
		auto it = _content.begin();
		_content.insert(it, data);
	}
}

// }}}
// {{{ void Response::sendReply()

void Response::sendReply(const std::string &reason, int code, const std::string &data) {
	// 如果传递第三个参数将覆盖以前设置的body
	if (data != "") {
		_content.clear();
		_content.push_back(data);
	}
	std::string buffer = "";
	for (auto &t : _content) {
		evbuffer_add(_buffer.get(), t.c_str(), t.size());		
		buffer.append(t);
	}			

	std::string logBody;
	if (buffer.size() > 2048) {
		logBody = "{Body length > 1024} Part body: " + buffer.substr(0, 2048);
	} else {
		logBody = buffer;
	}
	LOG_INFO  << "Response: code [" << code << "] reason ["<< reason << 
				 "], data ["<< logBody << "]" << " size [" << buffer.size() << "]";
	_code = code;
	_bufferSize = buffer.size();
	evhttp_send_reply(_request, code, reason.c_str(), _buffer.get());
}

// }}}
}
}
