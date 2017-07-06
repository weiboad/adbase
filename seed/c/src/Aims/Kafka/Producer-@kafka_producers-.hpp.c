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
	bool send(std::string& topicName, int* partId, adbase::Buffer& message);
	void errorCallback(const std::string& topicName, int partId, const adbase::Buffer& message, const std::string& error);
private:
	AimsContext* _context;
};

}
}
#endif
//@ENDIF
