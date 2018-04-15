//@IF @kafkac || @kafkap
#include "Aims.hpp"

// {{{ Aims::Aims()

Aims::Aims(AimsContext* context) :
	_context(context) {
	_configure = _context->config;
}

// }}}
// {{{ Aims::~Aims()

Aims::~Aims() {
}

// }}}
// {{{ void Aims::run()

void Aims::run() {
	// 初始化 server
	init();

	//@IF @kafkac
	_kafkaConsumer->start();
	_state = RUNNING;
	//@ENDIF
	
	//@IF @kafkap
	_kafkaProducer->start();
	//@ENDIF
}

// }}}
// {{{ void Aims::init()

void Aims::init() {
	//@IF @kafkac
	initKafkaConsumer();
	//@ENDIF
	//@IF @kafkap
	initKafkaProducer();
	//@ENDIF
}

// }}}
// {{{ void Aims::stop()

void Aims::stop() {
	//@IF @kafkac
	if (_kafkaConsumer) {
		_kafkaConsumer->stop();
	}
	//@ENDIF
	//@IF @kafkap
	if (_kafkaProducer) {
		_kafkaProducer->stop();
	}
	//@ENDIF
}

// }}}
//@IF @kafkac
// {{{ void Aims::initKafkaConsumer()

void Aims::initKafkaConsumer() {
    std::unordered_map<std::string, std::string> configs;
	getConfig(configs);
	configs["group.id"] = _configure->groupId;
	configs["enabled_events"] = _configure->enabledEvents;
	configs["queued.min.messages"] = _configure->queuedMinMessages;
	configs["queued.max.messages.kbytes"] = _configure->queuedMaxSize;

    std::unordered_map<std::string, std::string> tconfigs;
	getTopicConfig(tconfigs);

	_kafkaConsumerCallback = std::shared_ptr<aims::kafka::Consumer>(new aims::kafka::Consumer(_context));
	std::vector<std::string> topicNames = adbase::explode(_configure->topicName, ',', true);
    LOG_INFO << "Topic list:" << _configure->topicName;

    _kafkaConsumer = std::shared_ptr<adbase::kafka::Consumer>(new adbase::kafka::Consumer(configs, tconfigs, topicNames));
    _kafkaConsumer->setMessageHandler(std::bind(&aims::kafka::Consumer::pull,
                _kafkaConsumerCallback,
                std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, std::placeholders::_4));
    _kafkaConsumer->setStatCallback(std::bind(&aims::kafka::Consumer::stat,
                _kafkaConsumerCallback,
                std::placeholders::_1, std::placeholders::_2));
}

// }}}
// {{{ void Aims::pause()

void Aims::pause() {
    if (_state == RUNNING) {
        _kafkaConsumer->pause();
        _state = PAUSE;
    }
}

// }}}
// {{{ void Aims::resume()

void Aims::resume() {
    if (_state == PAUSE) {
        _kafkaConsumer->resume();
        _state = RUNNING;
    }
}

// }}}
//{{{ void Aims::getTopicConfig()

void Aims::getTopicConfig(std::unordered_map<std::string, std::string>& configs) {
	configs["auto.commit.enabled"] = _configure->autoCommitEnabled;
	configs["auto.offset.reset"] = _configure->offsetReset;
	configs["auto.commit.interval.ms"] = _configure->commitInterval;
	configs["consume.callback.max.messages"] = _configure->consumeCallbackMaxMessages;
}

//}}}
//@ENDIF
//@IF @kafkap
// {{{ void Aims::initKafkaProducer()

void Aims::initKafkaProducer() {
    _kafkaProducerCallback = std::shared_ptr<aims::kafka::Producer>(new aims::kafka::Producer(_context));
    std::unordered_map<std::string, std::string> configs;
	getConfig(configs);

    _kafkaProducer = std::shared_ptr<adbase::kafka::Producer>(new adbase::kafka::Producer(configs, _configure->queueLength));
    _kafkaProducer->setSendHandler(std::bind(&aims::kafka::Producer::send,
                                               _kafkaProducerCallback,
                                               std::placeholders::_1, std::placeholders::_2,
                                               std::placeholders::_3));
    _kafkaProducer->setErrorHandler(std::bind(&aims::kafka::Producer::errorCallback,
                                               _kafkaProducerCallback,
                                               std::placeholders::_1, std::placeholders::_2,
                                               std::placeholders::_3, std::placeholders::_4));
}

// }}}
// {{{ adbase::kafka::Producer* Aims::getProducer()

adbase::kafka::Producer* Aims::getProducer() {
    return _kafkaProducer.get();
}

// }}}
//@ENDIF
//{{{ void Aims::getConfig()

void Aims::getConfig(std::unordered_map<std::string, std::string>& configs) {
    configs["metadata.broker.list"] = _configure->brokerList;
	configs["statistics.interval.ms"] = _configure->statInterval;
    configs["debug"] = _configure->kafkaDebug;
    configs["security.protocol"] = _configure->securityProtocol;
    configs["sasl.mechanisms"] = _configure->saslMechanisms;
    configs["sasl.kerberos.service.name"] = _configure->kerberosServiceName;
    configs["sasl.kerberos.principal"] = _configure->kerberosPrincipal;
    configs["sasl.kerberos.kinit.cmd"] = _configure->kerberosCmd;
    configs["sasl.kerberos.keytab"] = _configure->kerberosKeytab;
    configs["sasl.kerberos.min.time.before.relogin"] = _configure->kerberosMinTime;
    configs["sasl.username"] = _configure->saslUsername;
    configs["sasl.password"] = _configure->saslPassword;
}

//}}}
//@ENDIF
