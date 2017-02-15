#include <adbase/Head.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace head {
// {{{ Handler::Handler()

Handler::Handler(Interface* interface) :
	_interface(interface) {
}

// }}}
// {{{ Handler::~Handler()

Handler::~Handler() {
}

// }}}
// {{{ void Handler::onConnection()

void Handler::onConnection(const TcpConnectionPtr& conn) {
	LOG_TRACE << conn->localAddress().toIpPort() << " -> "
			  << conn->peerAddress().toIpPort();	
}

// }}}
// {{{ void Handler::onMessage()

void Handler::onMessage(const TcpConnectionPtr& conn, evbuffer* evbuf, Timestamp reciveTime) {	
	char buf[1];
	if (evbuffer_copyout(evbuf, buf, 1) != 1) {
		return;
	}

	if (static_cast<uint8_t>(buf[0]) == PROTOCOL_BINARY_REQ) {
		Binary binaryProcessor(_interface);
		binaryProcessor.processData(conn, evbuf);
	}
	LOG_DEBUG << "Recive time:" << reciveTime.toString();
	LOG_DEBUG << "Read buffer len:" << evbuffer_get_length(evbuf);
}

// }}}
// {{{ void Handler::onClose()

void Handler::onClose(const TcpConnectionPtr& conn) {
	LOG_TRACE << "Onclose:" << conn->getCookie();
}

// }}}
}
}
