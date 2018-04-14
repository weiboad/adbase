//@IF @adserver
/// 该程序自动生成，禁止修改
#include "AdServer.hpp"

// {{{ AdServer::AdServer()

AdServer::AdServer(AdServerContext* context) :
	_context(context) {
	_configure = _context->config;
	_base = _context->mainEventBase;
}

// }}}
// {{{ AdServer::~AdServer()

AdServer::~AdServer() {
	stop();
}

// }}}
// {{{ void AdServer::run()

void AdServer::run() {
	// 初始化 server
	init();
	if (_configure->isStartHttp && _http != nullptr) {
		_http->start(_configure->httpThreadNum);
	}
	if (_configure->isStartMc) {
		_mcServer->start(_configure->mcThreadNum);
	}
	//@IF @grpc
	if (_configure->isStartGrpc) {
		_grpcServer->start();
	}
	//@ENDIF
}

// }}}
// {{{ void AdServer::init()

void AdServer::init() {
	if (_configure->isStartHttp) {
		initHttp();
	}
	if (_configure->isStartMc) {
		initMc();
	}
	//@IF @grpc
	if (_configure->isStartGrpc) {
		initGrpc();
	}
	//@ENDIF
}

// }}}
// {{{ void AdServer::stop()

void AdServer::stop() {
	if (_configure->isStartHttp && _http) {
		LOG_TRACE << "Stop http server.";
		_http->stop();
	}

	if (_configure->isStartMc && _mcServer) {
		_mcServer->stop();
	}

	//@IF @grpc
    if (_configure->isStartGrpc && _grpcServer) {
        _grpcServer->stop();
    }
	//@ENDIF
}

// }}}
// {{{ void AdServer::initHttp()

void AdServer::initHttp() {
	adbase::TimeZone tz(8*3600, "CST");
	LOG_TRACE << "Init http server, host:"
			  << _configure->httpHost
			  << " port:"
			  << _configure->httpPort;
	adbase::http::Config config(_configure->httpHost, _configure->httpPort, _configure->httpTimeout);
	config.setTimeZone(tz);
	config.setServerName(_configure->httpServerName);
	config.setLogDir(_configure->httpAccessLogDir);
	config.setLogRollSize(_configure->httpAccesslogRollSize);
	_http = std::shared_ptr<adbase::http::Server>(new adbase::http::Server(config));
	_httpManager = std::shared_ptr<Http>(new Http(_context, _http.get()));
	_httpManager->addController();
	_http->setLocationFallback(std::bind(&AdServer::httpFallback, this, std::placeholders::_1));
}

// }}}
// {{{ std::string AdServer::httpFallback()

std::string AdServer::httpFallback(const std::string& url) {
	std::string result  = "/";
	std::string trimUrl = adbase::trim(url, "/");
	if (_httpManager != nullptr) {
		std::unordered_map<std::string, std::string> rewrites = _httpManager->rewrite();
		if (rewrites.find(trimUrl) != rewrites.end()) {
			return rewrites[trimUrl];	
		}
	}
	if (trimUrl.empty()) {
		return result + _configure->httpDefaultController + "/" + _configure->httpDefaultAction; 
	} else if (trimUrl.find("/") == std::string::npos) {
		return result + trimUrl + "/" + _configure->httpDefaultAction; 
	} else {
		return url;	
	}
}

// }}}
// {{{ void AdServer::initMc()

void AdServer::initMc() {
    // Mc server
	_mcProcessor = std::shared_ptr<McProcessor>(new McProcessor(_context));
	_mcAddr      = std::shared_ptr<adbase::InetAddress>(new adbase::InetAddress(_configure->mcHost, static_cast<uint16_t>(_configure->mcPort)));
	_mcServer    = std::shared_ptr<adbase::TcpServer>(new adbase::TcpServer(_base, *_mcAddr, _configure->mcServerName));
	_mcInterface = std::shared_ptr<adbase::mc::Interface>(new adbase::mc::Interface());
	bindMcCallback();
	_mcHandler   = std::shared_ptr<adbase::mc::Handler>(new adbase::mc::Handler(_mcInterface.get()));
    _mcServer->setConnectionCallback(std::bind(&adbase::mc::Handler::onConnection, _mcHandler, 
											   std::placeholders::_1));
    _mcServer->setCloseCallback(std::bind(&adbase::mc::Handler::onClose, _mcHandler, std::placeholders::_1));
    _mcServer->setMessageCallback(std::bind(&adbase::mc::Handler::onMessage, _mcHandler, std::placeholders::_1,
                							std::placeholders::_2, std::placeholders::_3));
}

// }}}
// {{{ void AdServer::bindMcCallback()

void AdServer::bindMcCallback() {
    _mcInterface->setAddHandler(std::bind(&McProcessor::add, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6, std::placeholders::_7));
    _mcInterface->setAppendHandler(std::bind(&McProcessor::append, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6));
    _mcInterface->setDecrementHandler(std::bind(&McProcessor::decrement, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6, std::placeholders::_7));
    _mcInterface->setDeleteHandler(std::bind(&McProcessor::deleteOp, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    _mcInterface->setFlushHandler(std::bind(&McProcessor::flush, _mcProcessor, std::placeholders::_1));
    _mcInterface->setGetHandler(std::bind(&McProcessor::get, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    _mcInterface->setIncrementHandler(std::bind(&McProcessor::increment, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6, std::placeholders::_7));
    _mcInterface->setPrependHandler(std::bind(&McProcessor::prepend, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6));
    _mcInterface->setNoopHandler(std::bind(&McProcessor::noop, _mcProcessor));
    _mcInterface->setQuitHandler(std::bind(&McProcessor::quit, _mcProcessor));
    _mcInterface->setReplaceHandler(std::bind(&McProcessor::replace, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6, std::placeholders::_7,
                std::placeholders::_8));
    _mcInterface->setSetHandler(std::bind(&McProcessor::set, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5,
                std::placeholders::_6, std::placeholders::_7,
                std::placeholders::_8));
    _mcInterface->setVersionHandler(std::bind(&McProcessor::version, _mcProcessor, std::placeholders::_1));
    _mcInterface->setVerbosityHandler(std::bind(&McProcessor::verbosity, _mcProcessor, std::placeholders::_1));
    _mcInterface->setStatHandler(std::bind(&McProcessor::stat, _mcProcessor, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));
    _mcInterface->setPreExecute(std::bind(&McProcessor::preExecute, _mcProcessor));
    _mcInterface->setPostExecute(std::bind(&McProcessor::postExecute, _mcProcessor));
    _mcInterface->setUnknownExecute(std::bind(&McProcessor::unknownExecute, _mcProcessor));
}

// }}}
//@IF @grpc
// {{{ void AdServer::initGrpc()

void AdServer::initGrpc() {
    _grpcServer =  std::shared_ptr<Grpc>(new Grpc(_context));
}

// }}}
//@ENDIF
//@ENDIF
