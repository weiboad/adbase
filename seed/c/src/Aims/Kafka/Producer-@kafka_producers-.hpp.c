//@IF @kafkap
#ifndef @ADINF_PROJECT_NAME|upper@_AIMS_KAFKA_PRODUER_@FILEREPLACE0|upper@HPP_
#define @ADINF_PROJECT_NAME|upper@_AIMS_KAFKA_PRODUER_@FILEREPLACE0|upper@HPP_

#include <adbase/Kafka.hpp>
#include <adbase/Logging.hpp>
#include "AdbaseConfig.hpp"

namespace aims {
namespace kafka {
class Producer@FILEREPLACE0@ {
public:
	Producer@FILEREPLACE0@(AimsContext* context);
	~Producer@FILEREPLACE0@();
	bool send(std::string& topicName, int* partId, adbase::Buffer& message, uint64_t* ackCode);
	void ackCallback(uint64_t ackCode);
	void errorCallback(uint64_t ackCode);
private:
	AimsContext* _context;
	uint64_t _index;
};

}
}
#endif
//@ENDIF
