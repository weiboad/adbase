#include <adbase/Head.hpp>

namespace adbase {
namespace head {
// {{{ Interface::Interface()

Interface::Interface() {
	
}

// }}}
// {{{ Interface::~Interface()

Interface::~Interface() {
	
}

// }}}
// {{{ void Interface::setReadHandler()

void Interface::setReadHandler(const ReadHandler& readHandler) {
	_readHandler = readHandler;	
}
 
// }}}
// {{{ const ReadHandler& Interface::getReadHandler()

const ReadHandler& Interface::getReadHandler() {
	return _readHandler;	
}

// }}}
}
}
