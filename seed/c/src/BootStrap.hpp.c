/// 该程序自动生成，禁止修改
#ifndef @ADINF_PROJECT_NAME|upper@_BOOTSTRAP_HPP_
#define @ADINF_PROJECT_NAME|upper@_BOOTSTRAP_HPP_

#include <signal.h>
#include <thread>
#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Net.hpp>
#include "AdbaseConfig.hpp"

class App;
class Timer;
//@IF @adserver
class AdServer;
//@ENDIF
//@IF @kafkac || @kafkap
class Aims;
//@ENDIF
class BootStrap {
public:
	BootStrap();
	~BootStrap();
	void init(int argc, char **argv);
	void run();
	void reload();
	void stop(const int sig);
	AdbaseConfig* getConfig();

private:
    std::shared_ptr<AdbaseConfig> _configure;
	mutable std::mutex _mut;
	//@IF @logging
    std::shared_ptr<adbase::AsyncLogging> _asnclog;
	//@ENDIF
	std::string _configFile;
	//@IF @adserver
    std::shared_ptr<AdServer> _adServer;
	//@ENDIF
    std::shared_ptr<adbase::EventLoop> _loop;
	//@IF @kafkap || @kafkac
    std::shared_ptr<Aims> _aims;
	//@ENDIF
    std::shared_ptr<Timer> _timer;
    std::unique_ptr<App> _app;

	void daemonInit();
	//@IF @logging
	void asyncLogger(const char* msg, int len);
	void loggerInit(const adbase::TimeZone& tz);
	void setLoggerLevel();
	//@ENDIF
	void loadConfig();
	void parseOption(int argc, char **argv);
	void usage();
	void printVersion();
};

#endif
