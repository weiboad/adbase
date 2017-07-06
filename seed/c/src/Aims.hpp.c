//@IF @kafkac || @kafkap
#ifndef @ADINF_PROJECT_NAME|upper@_AIMS_HPP_
#define @ADINF_PROJECT_NAME|upper@_AIMS_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Kafka.hpp>
#include "AdbaseConfig.hpp"
//@IF @kafkac
//@FOR @kafka_consumers
#include "Aims/Kafka/Consumer@REPLACE0@.hpp"
//@ENDFOR
//@ENDIF
//@IF @kafkap 
//@FOR @kafka_producers
#include "Aims/Kafka/Producer@REPLACE0@.hpp"
//@ENDFOR
//@ENDIF

#ifndef DECLARE_KAFKA_CONSUMER
#define DECLARE_KAFKA_CONSUMER(name) \
	adbase::kafka::Consumer* _kafkaConsumer##name = nullptr;\
	aims::kafka::Consumer##name* _kafkaConsumerCallback##name = nullptr;
#endif
#ifndef DECLARE_KAFKA_PRODUCER
#define DECLARE_KAFKA_PRODUCER(name) \
	adbase::kafka::Producer* _kafkaProducer##name = nullptr;\
	aims::kafka::Producer##name* _kafkaProducerCallback##name = nullptr;
#endif

class Aims {
public:
	Aims(AimsContext* context);
	~Aims();
	void run();
	void stop();

private:
	/// 传输上下文指针
	AimsContext* _context; 
	AdbaseConfig* _configure;

	//@IF @kafkac
	//@FOR @kafka_consumers
	DECLARE_KAFKA_CONSUMER(@REPLACE0@);
	//@ENDFOR
	//@ENDIF
	//@IF @kafkap
	//@FOR @kafka_producers
	DECLARE_KAFKA_PRODUCER(@REPLACE0@)
	//@ENDFOR
	//@ENDIF
	void init();
	//@IF @kafkac
	void initKafkaConsumer();
	//@ENDIF
	//@IF @kafkap
	void initKafkaProducer();
	//@ENDIF
};

#endif
//@ENDIF
