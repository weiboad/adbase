//@IF @kafkap
#ifndef @ADINF_PROJECT_NAME|upper@_AIMS_KAFKA_PRODUER_HPP_
#define @ADINF_PROJECT_NAME|upper@_AIMS_KAFKA_PRODUER_HPP_

#include <adbase/Kafka.hpp>
#include <adbase/Logging.hpp>
#include "AdbaseConfig.hpp"

namespace aims {
namespace kafka {
class Producer {
public:
	Producer(AimsContext* context);
	~Producer();
	bool send(std::string& topicName, int* partId, adbase::Buffer& message);
	void errorCallback(const std::string& topicName, int partId, const adbase::Buffer& message, const std::string& error);
private:
	AimsContext* _context;
};

}
}
#endif
//@ENDIF
