#if !defined ADBASE_HTTP_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_HTTP_RESPONSE_HPP_
#define ADBASE_HTTP_RESPONSE_HPP_

#include <evhttp.h>
#include <string>
#include <memory>
#include <vector>

namespace adbase {

/**
 * @addtogroup http adbase::Http
 */
/*@{*/


namespace http {
class Response {
public:
	Response(evhttp_request *req);

	/// 设置 header 信息
	int setHeader(const std::string &key, const std::string &value, bool isReplace = true);

	/// 添加 header 信息
	int addHeader(const std::string &key, const std::string &value);

	/// 设置响应 body 信息
	void setContent(const std::string &data);

	/// 追加响应 body 信息
	void appendContent(const std::string &data, bool isAfter = true);

	/// 发送响应信息
	void sendReply(const std::string &reason = "OK", int code = HTTP_OK, const std::string &data = "");

	/// 获取状态码
	int getCode() { return _code; }

	/// 获取响应数据大小
	size_t getBodySize() { return _bufferSize; }

	virtual ~Response() {}

private:
	evhttp_request *_request;
	std::unique_ptr<evbuffer, void (*)(evbuffer*)> _buffer;
	std::vector<std::string> _content {""};
	int _code = HTTP_OK;
	size_t _bufferSize = 0;
};	
}

/*@}*/

}
#endif
