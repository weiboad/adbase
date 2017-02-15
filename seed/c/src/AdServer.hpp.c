//@IF @adserver
/// 该程序自动生成，禁止修改
#ifndef @ADINF_PROJECT_NAME|upper@_ADSERVER_HPP_
#define @ADINF_PROJECT_NAME|upper@_ADSERVER_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Http.hpp>
#include <adbase/Net.hpp>
#include <adbase/Mc.hpp>
#include <adbase/Head.hpp>
#include "AdbaseConfig.hpp"
#include "Http.hpp"
#include "McProcessor.hpp"
#include "HeadProcessor.hpp"

class AdServer {
public:
	AdServer(AdServerContext* context);
	~AdServer();
	void run();
	std::string httpFallback(const std::string& url);

private:
	AdbaseConfig* _configure;
	/// 传输上下文指针
	AdServerContext* _context; 
	adbase::EventBasePtr _base;

	/// http server
	adbase::http::Server* _http   = nullptr;
	Http* _httpManager = nullptr;
	/// mc server
	McProcessor* _mcProcessor    = nullptr;
	adbase::TcpServer* _mcServer = nullptr;
	adbase::InetAddress* _mcAddr = nullptr;
	adbase::mc::Interface* _mcInterface = nullptr;
	adbase::mc::Handler* _mcHandler     = nullptr;
	/// head server
	HeadProcessor* _headProcessor  = nullptr;
	adbase::TcpServer* _headServer = nullptr;
	adbase::InetAddress* _headAddr = nullptr;
	adbase::head::Interface* _headInterface = nullptr;
	adbase::head::Handler* _headHandler	    = nullptr;

	void init();
	void stop();
	void initHttp();
	void initHead();
	void initMc();
	void bindMcCallback();
};

#endif
//@ENDIF
