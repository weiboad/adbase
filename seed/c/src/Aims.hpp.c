//@IF @kafkac || @kafkap
#ifndef @ADINF_PROJECT_NAME|upper@_AIMS_HPP_
#define @ADINF_PROJECT_NAME|upper@_AIMS_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Kafka.hpp>
#include "AdbaseConfig.hpp"
//@IF @kafkac
#include "Aims/Kafka/Consumer.hpp"
//@ENDIF
//@IF @kafkap 
#include "Aims/Kafka/Producer.hpp"
//@ENDIF

class Aims {
public:
	Aims(AimsContext* context);
	~Aims();
	void run();
	void stop();
	void reload();
	void pause();
	void resume();

	//@IF @kafkap
	adbase::kafka::Producer* getProducer();
	//@ENDIF

private:
	/// 传输上下文指针
	AimsContext* _context; 
	AdbaseConfig* _configure;

	bool _isResume = true;
	bool _isPause = true;

	//@IF @kafkac
	std::shared_ptr<aims::kafka::Consumer> _kafkaConsumerCallback;
	std::shared_ptr<adbase::kafka::Consumer> _kafkaConsumer;
	//@ENDIF
	//@IF @kafkap
	std::shared_ptr<adbase::kafka::Producer> _kafkaProducer;
	std::shared_ptr<aims::kafka::Producer> _kafkaProducerCallback;
	//@ENDIF
	void init();
	void getConfig(std::unordered_map<std::string, std::string>& configs);
	//@IF @kafkac
	void initKafkaConsumer();
	enum runState {
		STOP = 0,
		RUNNING = 1,
		PAUSE = 2,
	};
	runState _state = STOP;
	void getTopicConfig(std::unordered_map<std::string, std::string>& configs);
	//@ENDIF
	//@IF @kafkap
	void initKafkaProducer();
	//@ENDIF
};

#endif
//@ENDIF
