/// 该程序自动生成，禁止修改
#include <adbase/Logging.hpp>
#include <adbase/Metrics.hpp>
#include <adbase/Config.hpp>
#include <adbase/Net.hpp>
#include <unistd.h>
#include <signal.h>
#include "BootStrap.hpp"
#include "App.hpp"
#include "Version.hpp"
//@IF @adserver
#include "AdServer.hpp"
//@ENDIF
//@IF @kafkac || @kafkap
#include "Aims.hpp"
//@ENDIF
#include "Timer.hpp"

// {{{ BootStrap::BootStrap()

BootStrap::BootStrap() {
}

// }}}
// {{{ BootStrap::~BootStrap()

BootStrap::~BootStrap() {
}

// }}}
// {{{ void BootStrap::init()

void BootStrap::init(int argc, char **argv) {
    _configure = std::shared_ptr<AdbaseConfig>(new AdbaseConfig());
    std::unique_ptr<App> tmpApp(new App(_configure.get()));
    _app = std::move(tmpApp);

	// 解析指定的参数
	parseOption(argc, argv);
	loadConfig();
	// 时区设置
	adbase::TimeZone beijing(8*3600, "CST");
	// 初始化 daemon 程序
	daemonInit();
	//@IF @logging
	loggerInit(beijing);
	//@ENDIF
    _loop = std::shared_ptr<adbase::EventLoop>(new adbase::EventLoop());
}

// }}}
// {{{ void BootStrap::run()

void BootStrap::run() {
	// 创建定时器
	TimerContext timerContext;
    timerContext.config = _configure.get();
	timerContext.mainEventBase = _loop->getBase();
    _timer = std::shared_ptr<Timer>(new Timer(&timerContext));
	_timer->init();

	// 初始化 metric 信息
	adbase::metrics::Metrics* metrics = adbase::metrics::Metrics::init(_timer->getTimer());

	_app->run();
	_app->setTimerContext(&timerContext);

	//@IF @adserver
	// 构建 adserver 上下文
	// @todo
	AdServerContext context;
    context.config  = _configure.get();
	context.metrics = metrics;
	context.mainEventBase = _loop->getBase();
	_app->setAdServerContext(&context);
    _adServer = std::shared_ptr<AdServer>(new AdServer(&context));
	if (_adServer) {
		_adServer->run();
	}
	//@ENDIF
	
	//@IF @kafkac || @kafkap
	AimsContext aimsContext;
	aimsContext.config = _configure.get();
	_app->setAimsContext(&aimsContext);
    _aims = std::shared_ptr<Aims>(new Aims(&aimsContext));
	if (_aims) {
		_aims->run();
	}
	//@ENDIF
	
	_loop->start();
}

// }}}
// {{{ void BootStrap::reload()

void BootStrap::reload() {
	try {
		loadConfig();	
	} catch (...) {
		LOG_ERROR << "Reload config: " << _configFile << " is invalid, please check it.";
		return;
	}
	//@IF @logging
	setLoggerLevel();
	//@ENDIF
	_app->reload();
}

// }}}
// {{{ void BootStrap::stop()

void BootStrap::stop(const int sig) {
	LOG_ERROR << "Stop callback sig:" << sig;
	remove(_configure->pidFile.c_str());

	//@IF @kafkap || @kafkac
	if (_aims) {
		_aims->stop();
	}
	//@ENDIF
	
	if (_app) {
        _app->stop();
	}

	if (_loop) {
		_loop->stop();
	}
}

// }}}
//{{{ AdbaseConfig* BootStrap::getConfig()

AdbaseConfig* BootStrap::getConfig() {
	std::lock_guard<std::mutex> lk(_mut);
	return _configure.get();
}

//}}}
// {{{ void BootStrap::daemonInit()

void BootStrap::daemonInit() {
	if (_configure->daemon) {
		pid_t pid;
		pid = fork();
		if (pid < 0) {
			LOG_SYSFATAL << "Fork parent process fail.";
		}
		if (pid > 0) {
			exit(EXIT_SUCCESS);
		}
		setsid(); /* create a new session */
	}

	FILE *fpPidfile;
	fpPidfile = fopen(_configure->pidFile.c_str(), "w");
	fprintf(fpPidfile, "%d\n", getpid());
	fclose(fpPidfile);
}

// }}}
//@IF @logging
// {{{ void BootStrap::loggerInit()

void BootStrap::loggerInit(const adbase::TimeZone& tz) {
	adbase::Logger::setTimeZone(tz);
	if (_configure->isAsync) { // 只有是守护进程开启的时候日志开启异步写入
		adbase::Logger::setOutput(std::bind(&BootStrap::asyncLogger, this, 
											std::placeholders::_1, std::placeholders::_2));
		// 启动异步日志落地
		std::string basename = _configure->logsDir + std::string(::basename("@ADINF_PROJECT_NAME@"));
        _asnclog = std::shared_ptr<adbase::AsyncLogging>(new adbase::AsyncLogging(basename, _configure->logRollSize));
		_asnclog->start();
	}
	setLoggerLevel();
}

// }}}
// {{{ void BootStrap::asyncLogger()

void BootStrap::asyncLogger(const char* msg, int len) {
	if (_asnclog) {
		_asnclog->append(msg, static_cast<int>(len));
	}
}

// }}}
// {{{ void BootStrap::setLoggerLevel()

void BootStrap::setLoggerLevel() {
	switch (_configure->logLevel) {
		case 1:
			adbase::Logger::setLogLevel(adbase::Logger::TRACE);
			break;
		case 2:
			adbase::Logger::setLogLevel(adbase::Logger::DEBUG);
			break;
		case 3:
		default:
			adbase::Logger::setLogLevel(adbase::Logger::INFO);
	}
}

// }}}
//@ENDIF
//{{{ void BootStrap::loadConfig()

void BootStrap::loadConfig() {
	std::lock_guard<std::mutex> lk(_mut);
	adbase::IniConfig config = adbase::IniParse::loadFile(_configFile);

	_configure->daemon  = config.getOptionBool("system", "daemon");
	_configure->pidFile = config.getOption("system", "pidFile");
	_configure->appid   = config.getOptionUint32("system", "appid");
	_configure->macid   = config.getOptionUint32("system", "macid");

	//@IF @logging
	_configure->logsDir		= config.getOption("logging", "logsDir");
	_configure->logRollSize = config.getOptionUint32("logging", "logRollSize");
	_configure->logLevel	= config.getOptionUint32("logging", "logLevel");
	_configure->isAsync	    = config.getOptionBool("logging", "isAsync");
	//@ENDIF
	
	//@IF @adserver
	_configure->isStartMc   = config.getOptionBool("adserver", "mc");
	_configure->isStartHttp = config.getOptionBool("adserver", "http");
	//@IF @grpc
	_configure->isStartGrpc = config.getOptionBool("adserver", "grpc");
	//@ENDIF

	_configure->httpHost = config.getOption("http", "host");
	_configure->httpPort = config.getOptionUint32("http", "port");
	_configure->httpTimeout      = config.getOptionUint32("http", "timeout");
	_configure->httpThreadNum    = config.getOptionUint32("http", "threadNum");
	_configure->httpServerName   = config.getOption("http", "serverName");
	_configure->httpDefaultController = config.getOption("http", "defaultController");
	_configure->httpDefaultAction     = config.getOption("http", "defaultAction");
	_configure->httpAccessLogDir = config.getOption("http", "accessLogDir");
	_configure->httpAccesslogRollSize = config.getOptionUint32("http", "accesslogRollSize");

	//@IF @grpc
	_configure->grpcHost = config.getOption("grpc", "host");
	_configure->grpcPort = config.getOptionUint32("grpc", "port");
	//@ENDIF
	_configure->mcHost = config.getOption("mc", "host");
	_configure->mcPort = config.getOptionUint32("mc", "port");
	_configure->mcServerName   = config.getOption("mc", "serverName");
	_configure->mcThreadNum    = config.getOptionUint32("mc", "threadNum");
	//@ENDIF
	
	_app->loadConfig(config);
}

//}}}
// {{{ void BootStrap::usage()

void BootStrap::usage() {
	std::cout << "Usage: @ADINF_PROJECT_NAME@ [options...] <path>" << std::endl;
	std::cout << "\t-c: 主配置文件路径" << std::endl;
	std::cout << "\t-h: 帮助" << std::endl;
	exit(0);
}

// }}}
// {{{ void BootStrap::printVersion()

void BootStrap::printVersion() {
	std::cout << "VERSION  :  " << VERSION << std::endl;
	std::cout << "GIT SHA1 :  " << GIT_SHA1 << std::endl;
	std::cout << "GIT DIRTY:  " << GIT_DIRTY << std::endl;
	std::cout << "BUILD ID :  " << BUILD_ID << std::endl;
	exit(0);
}

// }}}
// {{{ void BootStrap::parseOption() 

void BootStrap::parseOption(int argc, char **argv) {
	int ch;
	while((ch = getopt(argc, argv, "c:hv")) != -1) {
		if (ch == 'c') {
			_configFile = optarg;
		} else if (ch == 'h') {
			usage();
		} else if (ch == 'v') {
			printVersion();
		}
	}
}

// }}}
