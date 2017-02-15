//@IF @kafkac
#include "Consumer@FILEREPLACE0@.hpp"

namespace aims {
namespace kafka {
// {{{	Consumer@FILEREPLACE0@::Consumer@FILEREPLACE0@()

Consumer@FILEREPLACE0@::Consumer@FILEREPLACE0@(AimsContext* context) :
	_context(context) {
}

// }}}
// {{{ Consumer@FILEREPLACE0@::~Consumer@FILEREPLACE0@()

Consumer@FILEREPLACE0@::~Consumer@FILEREPLACE0@() {
}

// }}}
// {{{ void Consumer@FILEREPLACE0@::pull()

bool Consumer@FILEREPLACE0@::pull(const std::string& topicName, int partId, uint64_t offset, const adbase::Buffer& data) {
	(void)topicName;
	(void)partId;
	(void)offset;
	LOG_INFO << std::string(data.peek(), data.readableBytes());
	return true;
}

// }}}
// {{{ void Consumer@FILEREPLACE0@::stat()

void Consumer@FILEREPLACE0@::stat(adbase::kafka::Consumer* consumer, const std::string& stats) {
    LOG_INFO << "Stats:" << stats.substr(0, 1024);
    LOG_INFO << consumer->getTopicName();
}

// }}}
}
}

//@ENDIF
