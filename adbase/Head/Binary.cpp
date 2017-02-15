#include <adbase/Head.hpp>
#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace head {
// {{{ Binary::Binary()

Binary::Binary(Interface* interface) :
	_interface(interface) {
}

// }}}
// {{{ Binary::~Binary()

Binary::~Binary() {
}

// }}}
// {{{ void Binary::processData()

void Binary::processData(const TcpConnectionPtr& conn, evbuffer* evbuf) {		
	while (1) {
		ssize_t len = evbuffer_get_length(evbuf);
		ssize_t headerLen = static_cast<ssize_t>(sizeof(ProtocolBinaryHeader));
		if (len < headerLen) {
			// 协议包数据不完整	
			LOG_TRACE << "Read request header len less " << headerLen << " real len:" << len;
			break;
		}

		ProtocolBinaryHeader header;
		if (evbuffer_copyout(evbuf, &header, headerLen) != headerLen) {
			// 读取错误，关闭连接
			LOG_ERROR << "Read buffer error, conn:" << conn->getCookie(); 
			conn->shutdown();
			return;
		}

		ssize_t bodyLen = static_cast<ssize_t>(networkToHost32(header.head.bodylen));
		if (bodyLen > len - headerLen) {
			// 协议包数据不完整	
			LOG_TRACE << "Read request body len less " << bodyLen << " real len:" << len;
			break;
		}

		if (evbuffer_drain(evbuf, headerLen) == -1) {
			// 读取错误，关闭连接
			LOG_ERROR << "Read header error, conn:" << conn->getCookie(); 
			conn->shutdown();
			break;
		}

		std::unique_ptr<char[]> buffer(new char[bodyLen]);
		if (evbuffer_copyout(evbuf, buffer.get(), bodyLen) != bodyLen) {
			// 读取错误，关闭连接
			LOG_TRACE << "Read request real len:" << len;
			LOG_ERROR << "Read body buffer error, conn:" << conn->getCookie(); 
			conn->shutdown();
			break;
		}

		if (evbuffer_drain(evbuf, bodyLen) == -1) {
			// 读取错误，关闭连接
			LOG_ERROR << "Read body buffer error, conn:" << conn->getCookie(); 
			conn->shutdown();
			break;
		}

		LOG_DEBUG << "Body len: " << bodyLen;
		LOG_DEBUG << "Header len: " << headerLen;
		
		ReadHandler readHandler = _interface->getReadHandler();
		if (readHandler) {
			ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
			Buffer responseBuffer;
			ProtocolBinaryDataType responseDataType;
			rval = readHandler(static_cast<ProtocolBinaryDataType>(header.head.datatype), buffer.get(), bodyLen, &responseDataType, &responseBuffer);
			ProtocolBinaryHeader response;
			response.head.magic     = PROTOCOL_BINARY_RES;
			response.head.datatype  = responseDataType;
			response.head.masterversion  = header.head.masterversion;
			response.head.secondversion  = header.head.secondversion;
			response.head.opaque = header.head.opaque;
			response.head.status = hostToNetwork16(rval);
			response.head.appkey = header.head.appkey;
			uint32_t textlen = static_cast<uint32_t>(responseBuffer.readableBytes());
			response.head.bodylen = hostToNetwork32(textlen);

			conn->send(response.bytes, sizeof(response.bytes));
			if (textlen != 0) {
				conn->send(responseBuffer.peek(), textlen);
				responseBuffer.retrieveAll();
			}
		}

		LOG_DEBUG << "Proccess complete.";
	}
}

// }}}
}
}
