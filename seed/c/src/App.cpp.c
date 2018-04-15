#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include "App.hpp"

//{{{ macros

#define LOAD_TIMER_CONFIG(name) do {\
	_configure->interval##name = config.getOptionUint32("timer", "interval"#name);\
} while(0)

//}}}
// {{{ App::App()

App::App(AdbaseConfig* config) :
	_configure(config) {
}

// }}}
// {{{ App::~App()

App::~App() {
}

// }}}
// {{{ void App::run()

void App::run() {
}

// }}}
// {{{ void App::reload()

void App::reload() {
}

// }}}
// {{{ void App::stop()

void App::stop() {
}

// }}}
//@IF @adserver
// {{{ void App::setAdServerContext()

void App::setAdServerContext(AdServerContext* context) {
	context->app = this;
}

// }}}
//@ENDIF
//@IF @kafkac || @kafkap
// {{{ void App::setAimsContext()

void App::setAimsContext(AimsContext* context) {
	context->app = this;
}

// }}}
//@ENDIF
// {{{ void App::setTimerContext()

void App::setTimerContext(TimerContext* context) {
	context->app = this;
}

// }}}
//{{{ void App::loadConfig()

void App::loadConfig(adbase::IniConfig& config) {
	//@IF !@kafkap && !@kafkac && !@timer
	(void)config;
	//@ENDIF
    //@IF @kafkac || @kafkap
    _configure->brokerList = config.getOption("kafka", "brokerList");
    _configure->kafkaDebug = config.getOption("kafka", "debug");
	_configure->statInterval = config.getOption("kafka", "statInterval");
    //@IF @kafkap
    _configure->queueLength  = config.getOptionUint32("kafka", "queueLength");
    //@ENDIF
    //@IF @kafkac
    _configure->topicName    = config.getOption("kafka", "topicName");
    _configure->groupId      = config.getOption("kafka", "groupId");
	_configure->autoCommitEnabled = config.getOption("kafka", "autoCommitEnabled");
	_configure->offsetReset       = config.getOption("kafka", "offsetReset");
	_configure->commitInterval    = config.getOption("kafka", "commitInterval");
	_configure->enabledEvents     = config.getOption("kafka", "enabledEvents");
	_configure->queuedMinMessages = config.getOption("kafka", "queuedMinMessages");
	_configure->queuedMaxSize     = config.getOption("kafka", "queuedMaxSize");
	_configure->consumeCallbackMaxMessages = config.getOption("kafka", "consumeCallbackMaxMessages");
    //@ENDIF
	_configure->securityProtocol = config.getOption("kafka", "securityProtocol");
	_configure->saslMechanisms = config.getOption("kafka", "saslMechanisms");
	_configure->kerberosServiceName = config.getOption("kafka", "kerberosServiceName");
	_configure->kerberosPrincipal = config.getOption("kafka", "kerberosPrincipal");
	_configure->kerberosCmd = config.getOption("kafka", "kerberosCmd");
	_configure->kerberosKeytab = config.getOption("kafka", "kerberosKeytab");
	_configure->kerberosMinTime = config.getOption("kafka", "kerberosMinTime");
	_configure->saslUsername = config.getOption("kafka", "saslUsername");
	_configure->saslPassword = config.getOption("kafka", "saslPassword");
    //@ENDIF
	
    //@IF @timer
    //@FOR @timers
	LOAD_TIMER_CONFIG(@REPLACE0|ucfirst@);
    //@ENDFOR
    //@ENDIF
}

//}}}
//{{{ uint64_t App::getSeqId()

uint64_t App::getSeqId() {
	std::lock_guard<std::mutex> lk(_mut);
	adbase::Sequence seq;
	return seq.getSeqId(static_cast<uint16_t>(_configure->macid), static_cast<uint16_t>(_configure->appid));
}

//}}}
