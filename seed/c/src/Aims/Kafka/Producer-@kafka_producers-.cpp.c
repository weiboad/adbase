//@IF @kafkap
#include "Producer@FILEREPLACE0@.hpp"

namespace aims {
namespace kafka {
// {{{	Producer@FILEREPLACE0@::Producer@FILEREPLACE0@()

Producer@FILEREPLACE0@::Producer@FILEREPLACE0@(AimsContext* context) :
	_context(context),
	_index(0) {
}

// }}}
// {{{ Producer@FILEREPLACE0@::~Producer@FILEREPLACE0@()

Producer@FILEREPLACE0@::~Producer@FILEREPLACE0@() {
}

// }}}
// {{{ bool Producer@FILEREPLACE0@::send()

bool Producer@FILEREPLACE0@::send(std::string& topicName, int* partId, adbase::Buffer& data, uint64_t* ackCode) {
	// todo send kafka message
	topicName = _context->config->topicNameProducer@FILEREPLACE0@;
	if (_index >= std::numeric_limits<uint64_t>::max()) {
		_index = 0;	
	}
	*ackCode = _index++;
	*partId  = 0;
	data.append("TESTDEBUG");
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return true;
}

// }}}
// {{{ void Producer@FILEREPLACE0@::ackCallback()

void Producer@FILEREPLACE0@::ackCallback(uint64_t ackCode) {
	LOG_INFO << "Ack send ackCode: " << ackCode;
}

// }}}
// {{{ void Producer@FILEREPLACE0@::errorCallback()

void Producer@FILEREPLACE0@::errorCallback(uint64_t ackCode) {
	LOG_ERROR << "Error send ackCode: " << ackCode;
}

// }}}
}
}

//@ENDIF
