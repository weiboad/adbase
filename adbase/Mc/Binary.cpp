#include <adbase/Mc.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Utility.hpp>

namespace adbase {
namespace mc {
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
		ssize_t headerLen = static_cast<ssize_t>(sizeof(ProtocolBinaryRequestHeader));
		if (len < headerLen) {
			// 协议包数据不完整	
			LOG_TRACE << "Read request header len less " << headerLen << " real len:" << len;
			break;
		}

		ProtocolBinaryRequestHeader header;
		if (evbuffer_copyout(evbuf, &header, headerLen) != headerLen) {
			// 读取错误，关闭连接
			LOG_ERROR << "Read buffer error, conn:" << conn->getCookie(); 
			conn->shutdown();
			return;
		}

		ssize_t bodyLen = static_cast<ssize_t>(networkToHost32(header.request.bodylen));
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

		uint8_t cmdOp = header.request.opcode;
		ProtocolBinaryCommand cmd = static_cast<ProtocolBinaryCommand>(cmdOp);
		printCmd(cmd);
		LOG_DEBUG << "Body len: " << bodyLen;
		LOG_DEBUG << "Header len: " << headerLen;
		
		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND; 
		switch (cmd) {
			case PROTOCOL_BINARY_CMD_VERSION:
				rval = processVersion(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_SET:
				rval = processSet(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_ADD:
			case PROTOCOL_BINARY_CMD_ADDQ:
				rval = processAdd(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_APPEND:
			case PROTOCOL_BINARY_CMD_APPENDQ:
				rval = processAppend(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_PREPEND:
			case PROTOCOL_BINARY_CMD_PREPENDQ:
				rval = processPrepend(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_REPLACE:
			case PROTOCOL_BINARY_CMD_REPLACEQ:
				rval = processReplace(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_DECREMENT:
			case PROTOCOL_BINARY_CMD_DECREMENTQ:
				rval = processDecrement(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_INCREMENT:
			case PROTOCOL_BINARY_CMD_INCREMENTQ:
				rval = processIncrement(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_DELETE:
			case PROTOCOL_BINARY_CMD_DELETEQ:
				rval = processDelete(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_FLUSH:
			case PROTOCOL_BINARY_CMD_FLUSHQ:
				rval = processFlush(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_GET:
			case PROTOCOL_BINARY_CMD_GETK:
			case PROTOCOL_BINARY_CMD_GETQ:
			case PROTOCOL_BINARY_CMD_GETKQ:
				rval = processGet(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_NOOP:
				rval = processNoop(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_STAT:
				rval = processStat(conn, header, buffer.get());
				break;
			case PROTOCOL_BINARY_CMD_QUIT:
			case PROTOCOL_BINARY_CMD_QUITQ:
				rval = processNoop(conn, header, buffer.get());
				break;
			default:
				rval = PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND;
		}

		if (rval != PROTOCOL_BINARY_RESPONSE_SUCCESS) {
			ProtocolBinaryResponseNoExtras response;
			memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
			response.message.header.response.magic= PROTOCOL_BINARY_RES;
			response.message.header.response.opcode= header.request.opcode;
			response.message.header.response.status= htons(rval);
			response.message.header.response.opaque= header.request.opaque;
			conn->send(response.bytes, sizeof(response.bytes));
		}
		LOG_DEBUG << "Proccess complete.";
	}
}

// }}}
// {{{ void Binary::printCmd()

void Binary::printCmd(ProtocolBinaryCommand cmd) {
	std::string cmdStr;
	switch (cmd) {
		case PROTOCOL_BINARY_CMD_GET: cmdStr = "PROTOCOL_BINARY_CMD_GET";break;
		case PROTOCOL_BINARY_CMD_SET: cmdStr = "PROTOCOL_BINARY_CMD_SET";break;
		case PROTOCOL_BINARY_CMD_ADD: cmdStr = "PROTOCOL_BINARY_CMD_ADD";break;
		case PROTOCOL_BINARY_CMD_REPLACE: cmdStr = "PROTOCOL_BINARY_CMD_REPLACE";break;
		case PROTOCOL_BINARY_CMD_DELETE: cmdStr = "PROTOCOL_BINARY_CMD_DELETE";break;
		case PROTOCOL_BINARY_CMD_INCREMENT: cmdStr = "PROTOCOL_BINARY_CMD_INCREMENT";break;
		case PROTOCOL_BINARY_CMD_DECREMENT: cmdStr = "PROTOCOL_BINARY_CMD_DECREMENT";break;
		case PROTOCOL_BINARY_CMD_QUIT: cmdStr = "PROTOCOL_BINARY_CMD_QUIT";break;
		case PROTOCOL_BINARY_CMD_FLUSH: cmdStr = "PROTOCOL_BINARY_CMD_FLUSH";break;
		case PROTOCOL_BINARY_CMD_GETQ: cmdStr = "PROTOCOL_BINARY_CMD_GETQ";break;
		case PROTOCOL_BINARY_CMD_NOOP: cmdStr = "PROTOCOL_BINARY_CMD_NOOP";break;
		case PROTOCOL_BINARY_CMD_VERSION: cmdStr = "PROTOCOL_BINARY_CMD_VERSION";break;
		case PROTOCOL_BINARY_CMD_GETK: cmdStr = "PROTOCOL_BINARY_CMD_GETK";break;
		case PROTOCOL_BINARY_CMD_GETKQ:cmdStr = "PROTOCOL_BINARY_CMD_GETKQ";break;
		case PROTOCOL_BINARY_CMD_APPEND: cmdStr = "PROTOCOL_BINARY_CMD_APPEND";break;
		case PROTOCOL_BINARY_CMD_PREPEND: cmdStr = "PROTOCOL_BINARY_CMD_PREPEND";break;
		case PROTOCOL_BINARY_CMD_STAT: cmdStr = "PROTOCOL_BINARY_CMD_STAT";break;
		case PROTOCOL_BINARY_CMD_SETQ: cmdStr = "PROTOCOL_BINARY_CMD_SETQ";break;
		case PROTOCOL_BINARY_CMD_ADDQ: cmdStr = "PROTOCOL_BINARY_CMD_ADDQ";break;
		case PROTOCOL_BINARY_CMD_REPLACEQ: cmdStr = "PROTOCOL_BINARY_CMD_REPLACEQ";break;
		case PROTOCOL_BINARY_CMD_DELETEQ: cmdStr = "PROTOCOL_BINARY_CMD_DELETEQ";break;
		case PROTOCOL_BINARY_CMD_INCREMENTQ: cmdStr = "PROTOCOL_BINARY_CMD_INCREMENTQ";break;
		case PROTOCOL_BINARY_CMD_DECREMENTQ: cmdStr = "PROTOCOL_BINARY_CMD_DECREMENTQ";break;
		case PROTOCOL_BINARY_CMD_QUITQ: cmdStr = "PROTOCOL_BINARY_CMD_QUITQ";break;
		case PROTOCOL_BINARY_CMD_FLUSHQ: cmdStr = "PROTOCOL_BINARY_CMD_FLUSHQ";break;
		case PROTOCOL_BINARY_CMD_APPENDQ: cmdStr = "PROTOCOL_BINARY_CMD_APPENDQ";break;
		case PROTOCOL_BINARY_CMD_PREPENDQ: cmdStr = "PROTOCOL_BINARY_CMD_PREPENDQ";break;
		case PROTOCOL_BINARY_CMD_VERBOSITY: cmdStr = "PROTOCOL_BINARY_CMD_VERBOSITY";break;
		case PROTOCOL_BINARY_CMD_TOUCH: cmdStr = "PROTOCOL_BINARY_CMD_TOUCH";break;
		case PROTOCOL_BINARY_CMD_GAT: cmdStr = "PROTOCOL_BINARY_CMD_GAT";break;
		case PROTOCOL_BINARY_CMD_GATQ: cmdStr = "PROTOCOL_BINARY_CMD_GATQ";break;
		case PROTOCOL_BINARY_CMD_GATK: cmdStr = "PROTOCOL_BINARY_CMD_GATK";break;
		case PROTOCOL_BINARY_CMD_GATKQ: cmdStr = "PROTOCOL_BINARY_CMD_GATKQ";break;
		case PROTOCOL_BINARY_CMD_SASL_LIST_MECHS: cmdStr = "PROTOCOL_BINARY_CMD_SASL_LIST_MECHS";break;
		case PROTOCOL_BINARY_CMD_SASL_AUTH: cmdStr = "PROTOCOL_BINARY_CMD_SASL_AUTH";break;
		case PROTOCOL_BINARY_CMD_SASL_STEP: cmdStr = "PROTOCOL_BINARY_CMD_SASL_STEP";break;
		case PROTOCOL_BINARY_CMD_RGET:      cmdStr = "PROTOCOL_BINARY_CMD_RGET";break;
		case PROTOCOL_BINARY_CMD_RSET:      cmdStr = "PROTOCOL_BINARY_CMD_RSET";break;
		case PROTOCOL_BINARY_CMD_RSETQ:     cmdStr = "PROTOCOL_BINARY_CMD_RSETQ";break;
		case PROTOCOL_BINARY_CMD_RAPPEND:   cmdStr = "PROTOCOL_BINARY_CMD_RAPPEND";break;
		case PROTOCOL_BINARY_CMD_RAPPENDQ:  cmdStr = "PROTOCOL_BINARY_CMD_RAPPENDQ";break;
		case PROTOCOL_BINARY_CMD_RPREPEND:  cmdStr = "PROTOCOL_BINARY_CMD_RPREPEND";break;
		case PROTOCOL_BINARY_CMD_RPREPENDQ: cmdStr = "PROTOCOL_BINARY_CMD_RPREPENDQ";break;
		case PROTOCOL_BINARY_CMD_RDELETE:   cmdStr = "PROTOCOL_BINARY_CMD_RDELETE";break;
		case PROTOCOL_BINARY_CMD_RDELETEQ:  cmdStr = "PROTOCOL_BINARY_CMD_RDELETEQ";break;
		case PROTOCOL_BINARY_CMD_RINCR:     cmdStr = "PROTOCOL_BINARY_CMD_RINCR";break;
		case PROTOCOL_BINARY_CMD_RINCRQ:    cmdStr = "PROTOCOL_BINARY_CMD_RINCRQ";break;
		case PROTOCOL_BINARY_CMD_RDECR:     cmdStr = "PROTOCOL_BINARY_CMD_RDECR";break;
		case PROTOCOL_BINARY_CMD_RDECRQ:    cmdStr = "PROTOCOL_BINARY_CMD_RDECRQ";break;
		case PROTOCOL_BINARY_CMD_SET_VBUCKET: cmdStr = "PROTOCOL_BINARY_CMD_SET_VBUCKET";break;
		case PROTOCOL_BINARY_CMD_GET_VBUCKET: cmdStr = "PROTOCOL_BINARY_CMD_GET_VBUCKET";break;
		case PROTOCOL_BINARY_CMD_DEL_VBUCKET: cmdStr = "PROTOCOL_BINARY_CMD_DEL_VBUCKET";break;
		case PROTOCOL_BINARY_CMD_TAP_CONNECT:  cmdStr = "PROTOCOL_BINARY_CMD_TAP_CONNECT";break;
		case PROTOCOL_BINARY_CMD_TAP_MUTATION: cmdStr = "PROTOCOL_BINARY_CMD_TAP_MUTATION";break;
		case PROTOCOL_BINARY_CMD_TAP_DELETE:   cmdStr = "PROTOCOL_BINARY_CMD_TAP_DELETE";break;
		case PROTOCOL_BINARY_CMD_TAP_FLUSH:    cmdStr = "PROTOCOL_BINARY_CMD_TAP_FLUSH";break;
		case PROTOCOL_BINARY_CMD_TAP_OPAQUE:   cmdStr = "PROTOCOL_BINARY_CMD_TAP_OPAQUE";break;
		case PROTOCOL_BINARY_CMD_TAP_VBUCKET_SET:      cmdStr = "PROTOCOL_BINARY_CMD_TAP_VBUCKET_SET";break;
		case PROTOCOL_BINARY_CMD_TAP_CHECKPOINT_START: cmdStr = "PROTOCOL_BINARY_CMD_TAP_CHECKPOINT_START";break;
		case PROTOCOL_BINARY_CMD_TAP_CHECKPOINT_END:   cmdStr = "PROTOCOL_BINARY_CMD_TAP_CHECKPOINT_END";break;
		case PROTOCOL_BINARY_CMD_LAST_RESERVED: cmdStr = "PROTOCOL_BINARY_CMD_LAST_RESERVED";break;
		case PROTOCOL_BINARY_CMD_SCRUB: cmdStr = "PROTOCOL_BINARY_CMD_SCRUB";break;
		default:
			cmdStr = "PROTOCOL_BINARY_CMD_UNKNOWN";
	};
	
	LOG_DEBUG << cmdStr;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processVersion()

ProtocolBinaryResponseStatus Binary::processVersion(const TcpConnectionPtr& conn,
													ProtocolBinaryRequestHeader header, const char *buffer) {
	(void)buffer;
	VersionHandler version = _interface->getVersionHandler();
	if (version) {
		Buffer responseBuffer;
		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		version(&responseBuffer);
		uint32_t textlen = static_cast<uint32_t>(responseBuffer.readableBytes());
		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode= header.request.opcode;
		response.message.header.response.status= htons(rval);
		response.message.header.response.opaque= header.request.opaque;
		response.message.header.response.bodylen= htonl(textlen);
		response.message.header.response.cas= 0;

		conn->send(response.bytes, sizeof(response.bytes));
		conn->send(responseBuffer.peek(), textlen);
		responseBuffer.retrieveAll();
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processNoop()

ProtocolBinaryResponseStatus Binary::processNoop(const TcpConnectionPtr& conn,
												 ProtocolBinaryRequestHeader header, const char *buffer) {
	(void)buffer;
	NoopHandler noopHandler = _interface->getNoopHandler();
	if (noopHandler) {
		noopHandler();
	}
	ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
	ProtocolBinaryResponseNoExtras response;
	memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
	response.message.header.response.magic= PROTOCOL_BINARY_RES;
	response.message.header.response.opcode= header.request.opcode;
	response.message.header.response.status= htons(rval);
	response.message.header.response.opaque= header.request.opaque;

	conn->send(response.bytes, sizeof(response.bytes));
	return PROTOCOL_BINARY_RESPONSE_SUCCESS;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processSet()

ProtocolBinaryResponseStatus Binary::processSet(const TcpConnectionPtr& conn,
												ProtocolBinaryRequestHeader header, const char *buffer) {
    SetHandler setHandler = _interface->getSetHandler();
	if (setHandler) {
		uint16_t keylen  = networkToHost16(header.request.keylen);
		uint32_t datalen = networkToHost32(header.request.bodylen) - keylen - 8;
		char* bufferStart = const_cast<char*>(buffer);
		ProtocolBinaryRequestSetExtras* requestExtra = reinterpret_cast<ProtocolBinaryRequestSetExtras*>(bufferStart);
		uint32_t flags   = networkToHost32(requestExtra->flags);
		uint32_t timeout = networkToHost32(requestExtra->expiration);
		char *key= bufferStart + 8;
		char *data= key + keylen;
		uint64_t cas= networkToHost64(header.request.cas);
		uint64_t resultCas = 0;

		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		rval = setHandler(key, static_cast<uint16_t>(keylen), data, static_cast<uint32_t>(datalen),
						 flags, timeout, cas, &resultCas);
		
		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode= header.request.opcode;
		response.message.header.response.status= htons(rval);
		response.message.header.response.opaque= header.request.opaque;
		response.message.header.response.cas= hostToNetwork64(resultCas);

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processAdd()

ProtocolBinaryResponseStatus Binary::processAdd(const TcpConnectionPtr& conn,
												ProtocolBinaryRequestHeader header, const char *buffer) {
    AddHandler addHandler = _interface->getAddHandler();
	if (addHandler) {
		uint16_t keylen  = networkToHost16(header.request.keylen);
		uint32_t datalen = networkToHost32(header.request.bodylen) - keylen - 8;
		char* bufferStart = const_cast<char*>(buffer);
		ProtocolBinaryRequestAddExtras* requestExtra = reinterpret_cast<ProtocolBinaryRequestAddExtras*>(bufferStart);
		uint32_t flags= networkToHost32(requestExtra->flags);
		uint32_t timeout= networkToHost32(requestExtra->expiration);
		char *key= bufferStart + 8;
		char *data= key + keylen;
		uint64_t cas = 0;

		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		rval = addHandler(key, static_cast<uint16_t>(keylen), data, static_cast<uint32_t>(datalen),
						 flags, timeout, &cas);
		
		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode= header.request.opcode;
		response.message.header.response.status= htons(rval);
		response.message.header.response.opaque= header.request.opaque;
		response.message.header.response.cas= networkToHost64(cas);

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processAppend()

ProtocolBinaryResponseStatus Binary::processAppend(const TcpConnectionPtr& conn,
												   ProtocolBinaryRequestHeader header, const char *buffer) {
    AppendHandler appendHandler = _interface->getAppendHandler();
	if (appendHandler) {
		uint16_t keylen  = networkToHost16(header.request.keylen);
		uint32_t datalen = networkToHost32(header.request.bodylen) - keylen;
		char* key = const_cast<char*>(buffer);
		char* data = key + keylen;
		uint64_t cas = networkToHost64(header.request.cas);
		uint64_t resultCas = 0;

		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		rval = appendHandler(key, static_cast<uint16_t>(keylen), data, static_cast<uint32_t>(datalen),
						 	 cas, &resultCas);
		
		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode= header.request.opcode;
		response.message.header.response.status= htons(rval);
		response.message.header.response.opaque= header.request.opaque;
		response.message.header.response.cas= networkToHost64(resultCas);

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processPrepend()

ProtocolBinaryResponseStatus Binary::processPrepend(const TcpConnectionPtr& conn,
												    ProtocolBinaryRequestHeader header, const char *buffer) {
    PrependHandler prependHandler = _interface->getPrependHandler();
	if (prependHandler) {
		uint16_t keylen  = networkToHost16(header.request.keylen);
		uint32_t datalen = networkToHost32(header.request.bodylen) - keylen;
		char* key = const_cast<char*>(buffer);
		char* data = key + keylen;
		uint64_t cas = networkToHost64(header.request.cas);
		uint64_t resultCas = 0;

		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		rval = prependHandler(key, static_cast<uint16_t>(keylen), data, static_cast<uint32_t>(datalen),
						 	  cas, &resultCas);
		
		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode= header.request.opcode;
		response.message.header.response.status= htons(rval);
		response.message.header.response.opaque= header.request.opaque;
		response.message.header.response.cas= networkToHost64(resultCas);

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processReplace()

ProtocolBinaryResponseStatus Binary::processReplace(const TcpConnectionPtr& conn,
												    ProtocolBinaryRequestHeader header, const char *buffer) {
    ReplaceHander replaceHandler = _interface->getReplaceHandler();
	if (replaceHandler) {
		uint16_t keylen  = networkToHost16(header.request.keylen);
		uint32_t datalen = networkToHost32(header.request.bodylen) - keylen - 8;
		char* bufferStart = const_cast<char*>(buffer);
		ProtocolBinaryRequestReplaceExtras* requestExtra = reinterpret_cast<ProtocolBinaryRequestReplaceExtras*>(bufferStart);
		uint32_t flags= networkToHost32(requestExtra->flags);
		uint32_t timeout= networkToHost32(requestExtra->expiration);
		char *key= bufferStart + 8;
		char *data= key + keylen;
		uint64_t cas = networkToHost64(header.request.cas);
		uint64_t resultCas = 0;

		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		rval = replaceHandler(key, static_cast<uint16_t>(keylen), data, static_cast<uint32_t>(datalen),
						 	  flags, timeout, cas, &resultCas);
		
		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode= header.request.opcode;
		response.message.header.response.status= htons(rval);
		response.message.header.response.opaque= header.request.opaque;
		response.message.header.response.cas= networkToHost64(resultCas);

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processGet()

ProtocolBinaryResponseStatus Binary::processGet(const TcpConnectionPtr& conn,
												ProtocolBinaryRequestHeader header, const char *buffer) {
	GetHandler getHandler = _interface->getGetHandler();
	if (getHandler) {
		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		uint16_t keylen= networkToHost16(header.request.keylen);
		LOG_TRACE << keylen;
		uint64_t resultCas = 0;
		Buffer responseBuffer;
		rval = getHandler(buffer, keylen, &responseBuffer);

		uint32_t bodylen = static_cast<uint32_t>(responseBuffer.readableBytes());

		//uint8_t cmdOp = header.request.opcode;
		//ProtocolBinaryCommand cmd = static_cast<ProtocolBinaryCommand>(cmdOp);
		//if (cmd == PROTOCOL_BINARY_CMD_GET || cmd == PROTOCOL_BINARY_CMD_GETQ) {
		//	keylen = 0;	
		//}
		
		ProtocolBinaryResponseGet response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseGet));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode = header.request.opcode;
		response.message.header.response.status = htons(rval);
		response.message.header.response.opaque = header.request.opaque;
		response.message.header.response.cas    = hostToNetwork64(resultCas);
		response.message.header.response.keylen = hostToNetwork16(keylen);
		response.message.header.response.extlen = 4;
		response.message.header.response.bodylen = htonl(bodylen + keylen + 4);
		response.message.body.flags = htonl(0);

		conn->send(response.bytes, sizeof(response.bytes));
		LOG_TRACE << keylen;
		conn->send(buffer, keylen);
		conn->send(responseBuffer.peek(), bodylen);
		responseBuffer.retrieveAll();
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processDecrement()

ProtocolBinaryResponseStatus Binary::processDecrement(const TcpConnectionPtr& conn,
													  ProtocolBinaryRequestHeader header, const char *buffer) {
    DecrementHandler decrementHandler = _interface->getDecrementHandler();
	if (decrementHandler) {
		uint16_t keylen  = networkToHost16(header.request.keylen);
		char* bufferStart = const_cast<char*>(buffer);
		ProtocolBinaryRequestDecrExtras* requestExtra = reinterpret_cast<ProtocolBinaryRequestDecrExtras*>(bufferStart);
		uint64_t init  = networkToHost64(requestExtra->initial);
		uint64_t delta = networkToHost64(requestExtra->delta);
		uint32_t timeout = networkToHost32(requestExtra->expiration);
		char *key= bufferStart + sizeof(ProtocolBinaryRequestDecrExtras);
		uint64_t cas = 0;
		uint64_t result = 0;

		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		rval = decrementHandler(key, static_cast<uint16_t>(keylen), delta, init, timeout,
						 		&result, &cas);
		
		ProtocolBinaryResponseIncr response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseIncr));
		response.message.header.response.magic   = PROTOCOL_BINARY_RES;
		response.message.header.response.opcode  = header.request.opcode;
		response.message.header.response.status  = htons(rval);
		response.message.header.response.opaque  = header.request.opaque;
		response.message.header.response.cas     = networkToHost64(cas);
		response.message.header.response.bodylen = hostToNetwork32(8);
		response.message.body.value = hostToNetwork64(result);

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processIncrement()

ProtocolBinaryResponseStatus Binary::processIncrement(const TcpConnectionPtr& conn,
													  ProtocolBinaryRequestHeader header, const char *buffer) {
    IncrementHandler incrementHandler = _interface->getIncrementHandler();
	if (incrementHandler) {
		uint16_t keylen  = networkToHost16(header.request.keylen);
		char* bufferStart = const_cast<char*>(buffer);
		ProtocolBinaryRequestIncrExtras* requestExtra = reinterpret_cast<ProtocolBinaryRequestIncrExtras*>(bufferStart);
		uint64_t init  = networkToHost64(requestExtra->initial);
		uint64_t delta = networkToHost64(requestExtra->delta);
		uint32_t timeout = networkToHost32(requestExtra->expiration);
		char *key= bufferStart + sizeof(ProtocolBinaryRequestIncrExtras);
		uint64_t cas = 0;
		uint64_t result = 0;

		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		rval = incrementHandler(key, static_cast<uint16_t>(keylen), delta, init, timeout,
						 		&result, &cas);
		
		ProtocolBinaryResponseIncr response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseIncr));
		response.message.header.response.magic   = PROTOCOL_BINARY_RES;
		response.message.header.response.opcode  = header.request.opcode;
		response.message.header.response.status  = htons(rval);
		response.message.header.response.opaque  = header.request.opaque;
		response.message.header.response.cas     = networkToHost64(cas);
		response.message.header.response.bodylen = hostToNetwork32(8);
		response.message.body.value = hostToNetwork64(result);

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processDelete()

ProtocolBinaryResponseStatus Binary::processDelete(const TcpConnectionPtr& conn,
												   ProtocolBinaryRequestHeader header, const char *buffer) {
	DeleteHandler deleteHandler = _interface->getDeleteHandler();
	if (deleteHandler) {
		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		uint16_t keylen= networkToHost16(header.request.keylen);
		uint64_t cas = networkToHost64(header.request.cas);
		char* key = const_cast<char*>(buffer);
		rval = deleteHandler(key, keylen, cas);

		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode = header.request.opcode;
		response.message.header.response.status = htons(rval);
		response.message.header.response.opaque = header.request.opaque;

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processFlush()

ProtocolBinaryResponseStatus Binary::processFlush(const TcpConnectionPtr& conn,
												  ProtocolBinaryRequestHeader header, const char *buffer) {
	FlushHandler flushHandler = _interface->getFlushHandler();
	if (flushHandler) {
		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		char* bufferStart = const_cast<char*>(buffer);
		uint32_t timeout = 0;
		if (htonl(header.request.bodylen) == 4) {
			ProtocolBinaryRequestFlushExtras* requestExtra = reinterpret_cast<ProtocolBinaryRequestFlushExtras*>(bufferStart);
			timeout = networkToHost32(requestExtra->expiration);
		}
		rval = flushHandler(timeout);

		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode = header.request.opcode;
		response.message.header.response.status = htons(rval);
		response.message.header.response.opaque = header.request.opaque;

		conn->send(response.bytes, sizeof(response.bytes));
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processStat()

ProtocolBinaryResponseStatus Binary::processStat(const TcpConnectionPtr& conn,
												 ProtocolBinaryRequestHeader header, const char *buffer) {
	StatHandler statHandler = _interface->getStatHandler();
	if (statHandler) {
		ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
		uint16_t keylen= networkToHost16(header.request.keylen);
		char* key = const_cast<char*>(buffer);
		Buffer responseBuffer;
		rval = statHandler(key, keylen, &responseBuffer);

		uint32_t bodylen = static_cast<uint32_t>(responseBuffer.readableBytes());

		ProtocolBinaryResponseNoExtras response;
		memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
		response.message.header.response.magic= PROTOCOL_BINARY_RES;
		response.message.header.response.opcode = header.request.opcode;
		response.message.header.response.status = htons(rval);
		response.message.header.response.opaque = header.request.opaque;
		response.message.header.response.keylen = hostToNetwork16(keylen);
		response.message.header.response.bodylen = htonl(bodylen + keylen + 4);
		response.message.header.response.cas = 0;

		conn->send(response.bytes, sizeof(response.bytes));
		conn->send(key, keylen);
		conn->send(responseBuffer.peek(), bodylen);
		responseBuffer.retrieveAll();
		return PROTOCOL_BINARY_RESPONSE_SUCCESS;
	}

	return PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED;
}

// }}}
// {{{ ProtocolBinaryResponseStatus Binary::processQuit()

ProtocolBinaryResponseStatus Binary::processQuit(const TcpConnectionPtr& conn,
												 ProtocolBinaryRequestHeader header, const char *buffer) {
	(void)buffer;
	QuitHandler quitHandler = _interface->getQuitHandler();
	if (quitHandler) {
		quitHandler();
	}
	ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;	
	ProtocolBinaryResponseNoExtras response;
	memset(&response, 0, sizeof(ProtocolBinaryResponseNoExtras));
	response.message.header.response.magic= PROTOCOL_BINARY_RES;
	response.message.header.response.opcode= header.request.opcode;
	response.message.header.response.status= htons(rval);
	response.message.header.response.opaque= header.request.opaque;

	conn->send(response.bytes, sizeof(response.bytes));
	return PROTOCOL_BINARY_RESPONSE_SUCCESS;
}

// }}}
}
}
