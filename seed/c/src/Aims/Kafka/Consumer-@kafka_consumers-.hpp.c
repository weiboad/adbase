//@IF @kafkac
#ifndef @ADINF_PROJECT_NAME|upper@_AIMS_KAFKA_CONSUMER_@FILEREPLACE0|upper@HPP_
#define @ADINF_PROJECT_NAME|upper@_AIMS_KAFKA_CONSUMER_@FILEREPLACE0|upper@HPP_

#include <adbase/Kafka.hpp>
#include <adbase/Logging.hpp>
#include "AdbaseConfig.hpp"

namespace aims {
namespace kafka {
class Consumer@FILEREPLACE0@ {
public:
	Consumer@FILEREPLACE0@(AimsContext* context);
	~Consumer@FILEREPLACE0@();
	bool pull(const std::string& topicName, int partId, uint64_t offset, const adbase::Buffer& data);
	void stat(adbase::kafka::Consumer* consumer, const std::string& stats);
private:
	AimsContext* _context;
};

}
}
#endif
//@ENDIF
