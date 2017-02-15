//@IF @adserver
#ifndef @ADINF_PROJECT_NAME|upper@_HTTPINTERFACE_HPP_
#define @ADINF_PROJECT_NAME|upper@_HTTPINTERFACE_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Http.hpp>
#include "AdbaseConfig.hpp"

namespace adserver {
namespace http {
// {{{ ADSERVER_HTTP_ADD_API
class SourceFile {
public:
	template<int N>
	inline SourceFile(const char (&arr)[N]) :
		_data(arr),
		_size(N - 1) {
		const char* slash = strrchr(_data, '/');
		if (slash) {
			_data = slash + 1;
			_size -= static_cast<int>(_data - arr) + 4; // 后缀 .cpp
		}
	}

	const char* _data;
	int _size;
};
#ifndef ADSERVER_HTTP_ADD_API
#define ADSERVER_HTTP_ADD_API(http, class, api) do { \
    SourceFile file##api(__FILE__);\
    std::string module##api(file##api._data, file##api._size);\
    transform(module##api.begin(), module##api.end(), module##api.begin(), ::tolower);\
    std::string apiPath##api = "/" + module##api + "/"#api;\
    http->registerLocation(apiPath##api, std::bind(& class:: api, this, \
                std::placeholders::_1, \
                std::placeholders::_2, \
                std::placeholders::_3), nullptr); } while (0);
#endif
// }}}
class HttpInterface {
public:
	HttpInterface(AdServerContext* context);
	virtual ~HttpInterface();
	virtual void registerLocation(adbase::http::Server* http);

protected:
	/// 传输上下文指针
	AdServerContext* _context; 
	void responseHeader(adbase::http::Response* response);
	void responseJson(adbase::http::Response* response, const std::string& data, uint32_t ret, const std::string& msg, bool isRawString = false);
};
}
}
#endif
//@ENDIF
