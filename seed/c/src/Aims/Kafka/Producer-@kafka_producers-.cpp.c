//@IF @kafkap
#include "Producer@FILEREPLACE0@.hpp"

namespace aims {
namespace kafka {
// {{{	Producer@FILEREPLACE0@::Producer@FILEREPLACE0@()

Producer@FILEREPLACE0@::Producer@FILEREPLACE0@(AimsContext* context) :
	_context(context) {
}

// }}}
// {{{ Producer@FILEREPLACE0@::~Producer@FILEREPLACE0@()

Producer@FILEREPLACE0@::~Producer@FILEREPLACE0@() {
}

// }}}
// {{{ bool Producer@FILEREPLACE0@::send()

bool Producer@FILEREPLACE0@::send(std::string& topicName, int* partId, adbase::Buffer& data) {
	// todo send kafka message
	topicName = _context->config->topicNameProducer@FILEREPLACE0@;
	*partId  = 0;
	data.append("TESTDEBUG");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return true;
}

// }}}
// {{{ void Producer@FILEREPLACE0@::errorCallback()

void Producer@FILEREPLACE0@::errorCallback(const std::string& topicName, int partId, const adbase::Buffer& message, const std::string& error) {
	LOG_ERROR << "Error send error: " << error;
}

// }}}
}
}

//@ENDIF
