#ifndef @ADINF_PROJECT_NAME|upper@_ADBASE_CONFIG_HPP_
#define @ADINF_PROJECT_NAME|upper@_ADBASE_CONFIG_HPP_

#include <string>
#include <adbase/Net.hpp>
#include <adbase/Metrics.hpp>

// {{{ macros

#ifndef DECLARE_TIMER_CONFIG
#define DECLARE_TIMER_CONFIG(name) \
	int interval##name;
#endif

// }}}

typedef struct adbaseConfig {
	bool daemon;
	std::string pidFile;
	int appid;
	int macid;
	
	//@IF @logging
	// logging config
	std::string logsDir;
	size_t logRollSize;
	int logLevel;
	bool isAsync;
	//@ENDIF
	
	//@IF @adserver
	bool isStartMc;	
	bool isStartHttp;	
	//@IF @grpc
	bool isStartGrpc;
	//@ENDIF

	std::string httpHost;
	int httpPort;
	int httpTimeout;
	int httpThreadNum;
	std::string httpDefaultController;
	std::string httpDefaultAction;
	std::string httpServerName;
	std::string httpAccessLogDir;
	int httpAccesslogRollSize;

	//@IF @grpc
	std::string grpcHost;
	int grpcPort;
	//@ENDIF

	std::string mcHost;
	int mcPort;
	std::string mcServerName;
	int mcThreadNum;
	//@ENDIF
	//@IF @kafkac || @kafkap
	
	std::string brokerList;
	std::string kafkaDebug;
	std::string statInterval;
	//@IF @kafkap
	int queueLength;
	//@ENDIF
	//@IF @kafkac
	std::string topicName;
	std::string groupId;
	std::string autoCommitEnabled;
	std::string offsetReset;
	std::string consumeCallbackMaxMessages;
	std::string commitInterval;
	std::string enabledEvents;
	std::string queuedMinMessages;
	std::string queuedMaxSize;
	//@ENDIF
	std::string securityProtocol;
	std::string saslMechanisms;
	std::string kerberosServiceName;
	std::string kerberosPrincipal;
	std::string kerberosCmd;
	std::string kerberosKeytab;
	std::string kerberosMinTime;
	std::string saslUsername;
	std::string saslPassword;

	//@ENDIF
	//@IF @timer
	//@FOR @timers
	DECLARE_TIMER_CONFIG(@REPLACE0|ucfirst@);
	//@ENDFOR
	//@ENDIF
} AdbaseConfig;

class App;
//@IF @adserver
typedef struct adserverContext {
	AdbaseConfig* config;
	//@IF @adserver
	adbase::EventBasePtr mainEventBase;	
	//@ENDIF
	App* app;
	adbase::metrics::Metrics* metrics;
	// 前后端交互数据指针添加到下面
} AdServerContext;

//@ENDIF
//@IF @kafkac || @kafkap
typedef struct aimsContext {
	AdbaseConfig* config;
	App* app;
	// 消息队列交互上下文
} AimsContext;
//@ENDIF
typedef struct timerContext {
	AdbaseConfig* config;
	adbase::EventBasePtr mainEventBase;	
	App* app;
	// 定时器交互上下文
} TimerContext;

#endif
