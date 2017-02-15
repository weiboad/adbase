#include <adbase/Http.hpp>

namespace adbase {
namespace http {
// {{{ Request::Request()

Request::Request(evhttp_request *req) : 
	_request(req) {
}

// }}}
// {{{ Request::httpMethod Request::getMethod()

Request::httpMethod Request::getMethod() {
	enum evhttp_cmd_type type;
	type = evhttp_request_get_command(_request);
	switch (type) {
		case EVHTTP_REQ_GET:
			return METHOD_GET;
		case EVHTTP_REQ_POST:
			return METHOD_POST;
		default:
			return METHOD_OTHER;	
	}		
}

// }}}
// {{{const std::string Request::getUri() const

const std::string Request::getUri() const {
	const char *uri = evhttp_request_get_uri(_request);
	return std::string(uri);		
}

// }}}
// {{{ const std::string Request::getLocation() const

const std::string Request::getLocation() const {
	const char *uri = evhttp_request_get_uri(_request);
	std::size_t s = std::string(uri).find_first_of("?");
	std::string ret;
	if (s != std::string::npos) {
		ret = std::string(uri).substr(0, s);
	} else {
		ret = std::string(uri);	
	}
	return ret;		
}

// }}}
// {{{ const std::string Request::getHeader()

const std::string Request::getHeader(const std::string &key) const {
	struct evkeyvalq* evheader = evhttp_request_get_input_headers(_request);	
	const char *uri = evhttp_find_header(evheader, key.c_str());
	if (uri == nullptr) {
		return "";	
	} else {
		return std::string(uri);	
	}
}

// }}}
// {{{ const std::string Request::getQuery()

const std::string Request::getQuery(const std::string &key) const {
	const char *uri = evhttp_request_get_uri(_request);
	std::size_t s = std::string(uri).find_first_of("?");
	std::string ret;
	if (s != std::string::npos) {
		ret = std::string(uri).substr(s + 1);
	} else {
		return "";
	}
	return parseQuery(ret.c_str(), key);
}

// }}}
// {{{ const std::string Request::getPostData() const

const std::string Request::getPostData() const {
	struct evbuffer *buf = evhttp_request_get_input_buffer(_request);
	size_t len = evbuffer_get_length(buf);
	if (!len) {
		return "";	
	}
	std::unique_ptr<char []> tmp(new char[len + 1]);
	ev_ssize_t readsize = evbuffer_copyout(buf, tmp.get(), len);
	if (static_cast<unsigned int>(readsize) < len) {
		return "";
	}
	*(tmp.get() + len) = '\0';
	return std::string(tmp.get());
}

// }}}
// {{{ const std::string Request::getPost()

const std::string Request::getPost(const std::string &key) const {
	struct evbuffer *buf = evhttp_request_get_input_buffer(_request);	
	size_t len = evbuffer_get_length(buf);
	if (!len) {
		return "";	
	}
	std::unique_ptr<char []> tmp(new char[len + 1]);
	ev_ssize_t readsize = evbuffer_copyout(buf, tmp.get(), len);
	if (static_cast<unsigned int>(readsize) < len) {
		return "";
	}
	*(tmp.get() + len) = '\0';
	char *decode_uri = evhttp_decode_uri(tmp.get());

	std::string postData = parseQuery(decode_uri, key);
	free(decode_uri);
	return postData;
}

// }}}
// {{{ const std::string Request::getRemoteAddress()

const std::string Request::getRemoteAddress() const {
	char* remoteAddr = _request->remote_host;
	return std::string(remoteAddr);
}

// }}}
// {{{ const std::string Request::parseQuery() const

const std::string Request::parseQuery(const char *uri, const std::string &key) const {
	std::unique_ptr<evkeyvalq> evquery(new evkeyvalq);
	int ret = evhttp_parse_query_str(uri, evquery.get());	
	if (ret == -1) {
		return "";	
	}

	const char *value = evhttp_find_header(evquery.get(), key.c_str());
	
	std::string result;
	if (value != nullptr) {
		result = std::string(value);
	} else {
		result = "";
	}
	evhttp_clear_headers(evquery.get());

	return result;
}

// }}}
}
}
