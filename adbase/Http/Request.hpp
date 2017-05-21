#if !defined ADBASE_HTTP_HPP_
# error "Not allow include this header."
#endif

#ifndef ADBASE_HTTP_REQUEST_HPP_
#define ADBASE_HTTP_REQUEST_HPP_

#include <evhttp.h>
#include <string>
#include <memory>

namespace adbase {

/**
 * @addtogroup http adbase::Http
 */
/*@{*/

namespace http {

class Request {
public:
    enum httpMethod {
        METHOD_GET = 0,
        METHOD_POST = 1,
        METHOD_OTHER = 2
    };

    Request(evhttp_request *req);

    /// 获取请求方式
    Request::httpMethod getMethod();

    /// 获取请求 URI 地址
    const std::string getUri() const;

    /// 获取请求 Location 地址
    const std::string getLocation() const;

    /// 获取headers 信息
    const std::string getHeader(const std::string &key) const;

    /// 获取 GET 数据 信息
    const std::string getQuery(const std::string &key) const;

    /// 获取 POST FROM-TYPE 数据 信息
    const std::string getPost(const std::string &key) const;

    /// 获取 POST 元数据
    const std::string getPostData() const;

    /// 获取客户端IP
    const std::string getRemoteAddress() const;

    /// get server address
    const std::string getServerAddress() const;

    virtual ~Request() {}

private:
    const std::string parseQuery(const char *uri, const std::string &key) const;
    evhttp_request *_request;
};

}

/*@}*/

}
#endif
