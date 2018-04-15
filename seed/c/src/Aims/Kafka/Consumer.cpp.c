//@IF @kafkac
#include "Consumer.hpp"

namespace aims {
namespace kafka {
// {{{	Consumer::Consumer()

Consumer::Consumer(AimsContext* context) :
	_context(context) {
}

// }}}
// {{{ Consumer::~Consumer()

Consumer::~Consumer() {
}

// }}}
// {{{ void Consumer::pull()

bool Consumer::pull(const std::string& topicName, int partId, uint64_t offset, const adbase::Buffer& data) {
	(void)topicName;
	(void)partId;
	(void)offset;
	LOG_INFO << std::string(data.peek(), data.readableBytes());
	return true;
}

// }}}
// {{{ void Consumer::stat()

void Consumer::stat(adbase::kafka::Consumer* consumer, const std::string& stats) {
    LOG_INFO << "Stats:" << stats.substr(0, 1024);
}

// }}}
}
}

//@ENDIF
