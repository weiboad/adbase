//@IF @kafkap
#include "Producer.hpp"

namespace aims {
namespace kafka {
// {{{	Producer::Producer()

Producer::Producer(AimsContext* context) :
	_context(context) {
}

// }}}
// {{{ Producer::~Producer()

Producer::~Producer() {
}

// }}}
// {{{ bool Producer::send()

bool Producer::send(std::string& topicName, int* partId, adbase::Buffer& data) {
	// todo send kafka message
	topicName = "test";
	*partId  = 0;
	data.append("TESTDEBUG");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return true;
}

// }}}
// {{{ void Producer::errorCallback()

void Producer::errorCallback(const std::string& topicName, int partId, const adbase::Buffer& message, const std::string& error) {
	LOG_ERROR << "Error send error: " << error;
}

// }}}
}
}

//@ENDIF
