//@IF @kafkac
#ifndef @ADINF_PROJECT_NAME|upper@_AIMS_KAFKA_CONSUMER_HPP_
#define @ADINF_PROJECT_NAME|upper@_AIMS_KAFKA_CONSUMER_HPP_

#include <adbase/Kafka.hpp>
#include <adbase/Logging.hpp>
#include "AdbaseConfig.hpp"

namespace aims {
namespace kafka {
class Consumer {
public:
	Consumer(AimsContext* context);
	~Consumer();
	bool pull(const std::string& topicName, int partId, uint64_t offset, const adbase::Buffer& data);
	void stat(adbase::kafka::Consumer* consumer, const std::string& stats);
private:
	AimsContext* _context;
};

}
}
#endif
//@ENDIF
