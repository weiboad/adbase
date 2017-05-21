#define __USE_GNU       //启用CPU_ZERO等相关的宏  
#include <sched.h>  

#include <stdexcept>
#include <adbase/Http.hpp>
#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Metrics.hpp>
#include <cstdlib>

namespace adbase {
namespace http {

thread_local time_t httpLastSecond;
thread_local char httpLocalTime[32];

#pragma GCC diagnostic ignored "-Wwrite-strings"
static char  *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
#pragma GCC diagnostic warning "-Wwrite-strings"

namespace detail {
// {{{ void pipeHandler()

void pipeHandler(int, short evt, void *arg) {
    if (evt & EV_READ && arg != nullptr) {
		struct event_base* base = reinterpret_cast<event_base*>(arg);
		if (!event_base_got_break(base)) {
    		event_base_loopbreak(base);
			LOG_ERROR << "Stop http event loop.";
		}
    } else {
        LOG_ERROR << "Pipe event error.";
    }
}

// }}}
// {{{ void requestOnComplete()

void requestOnComplete(struct evhttp_request *req, void *data) {
    reinterpret_cast<Server*>(data)->onCompleteCallback(req);
}

// }}}
}
// {{{ std::string defaultFallback()

std::string defaultFallback(const std::string& url) {
	return url;
}

// }}}
// {{{ Server::Server()

Server::Server(Config httpConfig) : _config(httpConfig) {
	LOG_DEBUG << "Init Http Server.";
	_timeZone = _config.getTimeZone();
	_fallback = defaultFallback;
	_requestTimers = adbase::metrics::Metrics::buildTimers("adbase.http", "requests", _config.getStatInterval());
	_processTimers = adbase::metrics::Metrics::buildTimers("adbase.http", "process", _config.getStatInterval());
    adbase::metrics::Metrics::buildGauges("adbase.http", "simultaneous", _config.getStatInterval(), [this](){
        std::lock_guard<std::mutex> lk(_mut);
        return _requests.size();
    });
}

// }}}
// {{{ void httpServerOnRequestProxy()

void httpServerOnRequestProxy(evhttp_request *req, void *data) {
	reinterpret_cast<Server*>(data)->onRequestCallback(req);
}

// }}}
// {{{ void Server::onRequestCallback()

void Server::onRequestCallback(evhttp_request *req) {
    HttpRequest* httpRequest = new HttpRequest;
    Request* request = new Request(req);
    Response* response = new Response(req);
    httpRequest->request  = request;
    httpRequest->response = response;

    {
        std::lock_guard<std::mutex> lk(_mut);
        uintptr_t reqInt = reinterpret_cast<uintptr_t>(req);
        _requests[reqInt] = httpRequest;
        evhttp_request_set_on_complete_cb(req, detail::requestOnComplete, this);
    }

    // process
    adbase::metrics::Timer timer;
    timer.start();
    httpRequest->requestTimer.start();
    std::string requestUri =request->getLocation();
    if (_locations.find(requestUri) == _locations.end()) {
        requestUri = _fallback(requestUri);
    }
    auto it = _locations.find(requestUri);
    if (it == _locations.end()) {
        response->sendReply("Not found.", 404);
    } else {
        HttpLocation& process = it->second;
        try {
            process.process(request, response, process.data);
        } catch (std::exception &e) {                        LOG_WARN << "Process http request fail, exception: " <<  e.what();
            response->sendReply("Server Error.", 500);
        }
    }
    double time = timer.stop();
    httpRequest->upstreamTime = time;
}

// }}}
// {{{ void Server::onCompleteCallback()

void Server::onCompleteCallback(evhttp_request *req) {
    std::lock_guard<std::mutex> lk(_mut);
    uintptr_t reqInt = reinterpret_cast<uintptr_t>(req);
    if (_requests.find(reqInt) == _requests.end()) {
        return;
    }

    HttpRequest* httpRequest = _requests[reqInt];
    double requestTime = httpRequest->requestTimer.stop();
    HttpLog log;
    updateLocalTime();

    log.forwardedFor = "-";
    log.remoteAddr   = httpRequest->request->getRemoteAddress();
    log.remoteUser   = "-";
    log.timeLocal    = std::string(httpLocalTime);
    log.processTime  = std::to_string(httpRequest->upstreamTime);
    log.requestTime  = std::to_string(requestTime);

    std::string method = (httpRequest->request->getMethod() == Request::METHOD_GET) ? "GET" : "POST";
    log.request      = method + " " + httpRequest->request->getUri();
    log.status       = std::to_string(httpRequest->response->getCode());
    log.bodyBytesSent = std::to_string(httpRequest->response->getBodySize());
    log.httpReferer   = httpRequest->request->getHeader("Referer");
    log.httpUserAgent = httpRequest->request->getHeader("User-Agent");
    logger(log);

    if (_processTimers != nullptr) {
        _processTimers->setTimer(httpRequest->upstreamTime);
    }
    if (_requestTimers != nullptr) {
        _requestTimers->setTimer(requestTime);
    }

    delete httpRequest->request;
    delete httpRequest->response;
    delete httpRequest;
    _requests.erase(reqInt);
}

// }}}
// {{{ void Server::threadFunc()

void Server::threadFunc(int pipeFd) {
	{
		std::lock_guard<std::mutex> lk(_mut);
		cpu_set_t mask;  
		CPU_ZERO(&mask);      
		CPU_SET(_cpuMap[pipeFd], &mask);      //绑定cpu 1  
		if(sched_setaffinity(0, sizeof(mask), &mask) == -1) {  
			LOG_ERROR << "Set cpu affinity fail.";
		} 
	}
	LOG_DEBUG << "Start create base event.";
	std::unique_ptr<event_base, decltype(&event_base_free)> EventBase(event_base_new(), &event_base_free);
	if (!EventBase) {
		std::string errmsg = "Failed to create new base_event.";
		LOG_ERROR << errmsg;
		throw std::invalid_argument(errmsg); 
	}

	struct event pipeEvent;
	event_set(&pipeEvent, pipeFd, EV_READ | EV_PERSIST, detail::pipeHandler, EventBase.get());
	event_base_set(EventBase.get(), &pipeEvent);
	if (event_add(&pipeEvent, 0) == -1) {
		LOG_FATAL << "Set pipe event fail.";
	}
	LOG_TRACE << "Worker start.";

	LOG_DEBUG << "Start create new evhttp.";
	std::unique_ptr<evhttp, decltype(&evhttp_free)> EvHttp(evhttp_new(EventBase.get()), &evhttp_free);
	if (!EvHttp) {
		std::string errmsg = "Failed to create new evhttp.";
		LOG_ERROR << errmsg;
		throw std::invalid_argument(errmsg); 
	}
	LOG_DEBUG << "Start set evhttp onRequest callback.";
	evhttp_set_gencb(EvHttp.get(), httpServerOnRequestProxy, this);

	{
		std::lock_guard<std::mutex> lk(_mut);
		if (Socket == -1) {
			LOG_DEBUG << "Start bind evhttp socket, host:" <<  _config.getBindAddress()
					  << " port:" << _config.getBindPort();
			auto *BoundSock = evhttp_bind_socket_with_handle(EvHttp.get(), _config.getBindAddress().c_str(), 
							  static_cast<uint16_t>(_config.getBindPort()));
			if (!BoundSock) {
				std::string errmsg = "Failed to bind server socket.";
				LOG_ERROR << errmsg;
				throw std::invalid_argument(errmsg); 
			}
			if ((Socket = evhttp_bound_socket_get_fd(BoundSock)) == -1) {
				std::string errmsg = "Failed to get server socket for next instance.";
				LOG_ERROR << errmsg;
				throw std::invalid_argument(errmsg); 
			}
		} else {
			LOG_DEBUG << "Start set accecp evhttp socket.";
			if (evhttp_accept_socket(EvHttp.get(), Socket) == -1) {
				std::string errmsg = "Failed to bind server socket for new instance.";
				LOG_ERROR << errmsg;
				throw std::invalid_argument(errmsg); 
			}
		}
	}

	event_base_loop(EventBase.get(), 0);
}

// }}}
// {{{ void Server::start()

void Server::start(int threadNum) {
	int num = static_cast<int>(std::thread::hardware_concurrency());    //获取当前的cpu总数  
	for (int i = 0; i < threadNum; i++) {
		int fds[2];
		if (pipe(fds)) {
			LOG_FATAL << "Can't create stop pipe";
		}
		std::vector<int> pipefds;
		pipefds.push_back(fds[0]);
		pipefds.push_back(fds[1]);
    	_pipeMap[i] = pipefds;
		ThreadPtr Thread(new std::thread(std::bind(&Server::threadFunc, this, std::placeholders::_1), fds[0]), &Server::deleteThread);		
		std::lock_guard<std::mutex> lk(_mut);
		_cpuMap[fds[0]] = i % num;
		LOG_DEBUG << "Create worker thread success";
		Threads.push_back(std::move(Thread));
	}
	
	std::string fileName = _config.getLogDir() + "/" + _config.getServerName();
	_logger = new AsyncLogging(fileName, _config.getLogRollSize());
	_logger->start();
	LOG_DEBUG << "Create all worker thread, total " << threadNum << " thread.";
}

// }}}
// {{{ void Server::stop()

void Server::stop() {
	_running = false;
	if (_logger != nullptr) {
		delete _logger;
		_logger = nullptr;
	}
	LOG_ERROR << "stop http server";
	for (auto &t : _pipeMap) {
		char buf[1];
		buf[0] = 's';
		auto pipefds = t.second;
		if (write(pipefds[1], buf, 1) != 1) {
			LOG_ERROR << "Writing to thread stop pipe fail.";
		}
		LOG_ERROR << "notify http work thread stop.";
	}
}

// }}}
// {{{ void Server::deleteThread()

void Server::deleteThread(std::thread *t) {
	t->join();
	delete t;
	LOG_ERROR << "Delete http work thread.";
}

// }}}
// {{{ void Server::registerLocation()

void Server::registerLocation(const std::string &location, const OnRequest &httpLocal, void *data) {
	std::string trimLocation = trim(location);
	if (_locations.find(trimLocation) == _locations.end()) {
		HttpLocation item;
		item.process = httpLocal;
		item.data = data;
		_locations[trimLocation] = item;
		LOG_DEBUG << "Server Location: " << trimLocation << " register success.";
	} else {
		LOG_INFO << "Server Location: " << trimLocation << " is already register.";
	}
}

// }}}
// {{{ void Server::setLocationFallback()

void Server::setLocationFallback(const OnLocationFallback& fallback) {
	_fallback = fallback;
}

// }}}
// {{{ void Server::updateLocalTime()

void Server::updateLocalTime() {
	int64_t microSecondsSinceEpoch = Timestamp::now().microSecondsSinceEpoch();
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
	if (seconds != httpLastSecond) {
        httpLastSecond = seconds;
        struct tm tmTime;
        if (_timeZone.valid()) {
            tmTime = _timeZone.toLocalTime(seconds);
        } else {
            ::gmtime_r(&seconds, &tmTime);
        }

        snprintf(httpLocalTime, sizeof(httpLocalTime), 
				"%02d/%s/%d:%02d:%02d:%02d %c%02d%02d",
                tmTime.tm_mday, 
				months[tmTime.tm_mon],
				tmTime.tm_year + 1900,
                tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, 
				(tmTime.tm_gmtoff < 0) ? '-' : '+', 
				abs(static_cast<int>(tmTime.tm_gmtoff) / 3600), 
				abs(static_cast<int>(tmTime.tm_gmtoff) % 3600));
    }
}

// }}}
// {{{ void Server::logger()

void Server::logger(HttpLog& log) {
	std::string logTmp = _config.getLogFormat();
	int count;
	logTmp = replace("$http_x_forwarded_for", "-", logTmp, count);		
	logTmp = replace("$remote_addr", log.remoteAddr, logTmp, count);		
	logTmp = replace("$http_cip", "-", logTmp, count);		
	logTmp = replace("$remote_user", "-", logTmp, count);		
	logTmp = replace("$time_local", log.timeLocal, logTmp, count);		
	logTmp = replace("$request_time", log.requestTime, logTmp, count);		
	logTmp = replace("$upstream_response_time", log.processTime, logTmp, count);		
	logTmp = replace("$request", log.request, logTmp, count);		
	logTmp = replace("$status", log.status, logTmp, count);		
	logTmp = replace("$body_bytes_sent", log.bodyBytesSent, logTmp, count);		
	logTmp = replace("$http_referer", log.httpReferer, logTmp, count);		
	logTmp = replace("$http_user_agent", log.httpUserAgent, logTmp, count);		
	logTmp += "\n";
	
	if (_logger != nullptr) {
		_logger->append(logTmp.c_str(), static_cast<int>(logTmp.size()));
	}
}

// }}}
}
}
