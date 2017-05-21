#if !defined ADBASE_HTTP_HPP_
# error "Not allow include this header."
#endif

#ifndef ADBASE_HTTP_SERVER_HPP_
#define ADBASE_HTTP_SERVER_HPP_

#include <memory>
#include <evhttp.h>
#include <map>
#include <thread>
#include <mutex>
#include <functional>
#include <adbase/Metrics.hpp>

namespace adbase {

/**
 * @addtogroup http adbase::Http
 */
/*@{*/

class AsyncLogging;
namespace http {

class Config;
class Response;
class Request;
// onRequest
typedef std::function<void (Request* request,
                            Response* response,
                            void* data)> OnRequest;
/// 当请求
typedef std::function<const std::string (const std::string&)> OnLocationFallback;

class Server {
public:
    Server(Config httpConfig);
    /// 启动server
    void start(int threadNum = 1);
    void stop();
    /// 向 server 中注册location
    void registerLocation(const std::string &location, const OnRequest &process, void *data);
    void setLocationFallback(const OnLocationFallback& fallback);
    void onRequestCallback(evhttp_request *req);
    void onCompleteCallback(evhttp_request *req);
    void threadFunc(int pipeFd);
    static void deleteThread(std::thread *t);
    virtual ~Server() {}

private:
    bool _running = true;
    Config _config;
    std::unordered_map<int, std::vector<int>> _pipeMap;
    std::unordered_map<int, int> _cpuMap;
    typedef struct httpLocation {
        OnRequest process;
        void* data;
    } HttpLocation;

    typedef struct httpLog {
        std::string forwardedFor;
        std::string remoteAddr;
        std::string remoteUser;
        std::string timeLocal;
        std::string requestTime;
        std::string processTime;
        std::string request;
        std::string status;
        std::string bodyBytesSent;
        std::string httpReferer;
        std::string httpUserAgent;
    } HttpLog;

    typedef struct httpRequest {
        adbase::metrics::Timer requestTimer;
        double upstreamTime;
        Request* request;
        Response* response;
    } HttpRequest;

    std::unordered_map<uintptr_t, HttpRequest*> _requests;

    TimeZone _timeZone;
    AsyncLogging* _logger;
    std::map<std::string, httpLocation> _locations;
    OnLocationFallback _fallback;
    evutil_socket_t Socket = -1;
    mutable std::mutex _mut;
    typedef std::unique_ptr<std::thread, decltype(&Server::deleteThread)> ThreadPtr;
    typedef std::vector<ThreadPtr> ThreadPool;
    ThreadPool Threads;

    adbase::metrics::Timers* _processTimers = nullptr;
    adbase::metrics::Timers* _requestTimers = nullptr;

    void updateLocalTime();
    void logger(HttpLog& log);
};

}

/*@}*/

}
#endif
