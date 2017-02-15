#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include "App.hpp"

//{{{ macros

#define LOAD_KAFKA_CONSUMER_CONFIG(name, sectionName) do {\
    _configure->isNewConsumer##name = config.getOptionBool("kafkac_"#sectionName, "isNewConsumer"#name);\
    _configure->topicNameConsumer##name    = config.getOption("kafkac_"#sectionName, "topicName"#name);\
    _configure->groupId##name      = config.getOption("kafkac_"#sectionName, "groupId"#name);\
    _configure->brokerListConsumer##name   = config.getOption("kafkac_"#sectionName, "brokerList"#name);\
    _configure->kafkaDebug##name   = config.getOption("kafkac_"#sectionName, "kafkaDebug"#name);\
    _configure->offsetPath##name   = config.getOption("kafkac_"#sectionName, "offsetPath"#name);\
    _configure->statInterval##name = config.getOption("kafkac_"#sectionName, "statInterval"#name);\
} while(0)

#define LOAD_KAFKA_PRODUCER_CONFIG(name, sectionName) do {\
    _configure->topicNameProducer##name    = config.getOption("kafkap_"#sectionName, "topicName"#name);\
    _configure->brokerListProducer##name   = config.getOption("kafkap_"#sectionName, "brokerList"#name);\
    _configure->debug##name        = config.getOption("kafkap_"#sectionName, "debug"#name);\
    _configure->queueLength##name  = config.getOptionUint32("kafkap_"#sectionName, "queueLength"#name);\
} while(0)

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
    //@IF @kafkac
    //@FOR @kafka_consumers
	LOAD_KAFKA_CONSUMER_CONFIG(@REPLACE0@, @REPLACE0|lower@);
    //@ENDFOR
    //@ENDIF
   //@IF @kafkap
    //@FOR @kafka_producers
	LOAD_KAFKA_PRODUCER_CONFIG(@REPLACE0@, @REPLACE0|lower@);
    //@ENDFOR
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
