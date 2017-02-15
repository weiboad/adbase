#include <adbase/Mc.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace mc {
// {{{ ClientHandler::ClientHandler()

ClientHandler::ClientHandler(EventBasePtr& base, const std::string& hostname, int port,
							 Timer* timer, bool isRetry) :
	_base(base),
	_hostname(hostname),
	_port(port),
	_timer(timer),
	_isRetry(isRetry),
	_binary(true),
	_client(nullptr),
	_currentGetCmd(getHeaderState),
	_getDataSize(0) {
}

// }}}
// {{{ void ClientHandler::start()

void ClientHandler::start() {	
	_client = new TcpClient(_base, _hostname, _port, "MC-CLIENT", _timer);
	if (_isRetry) {
		_client->enableRetry();	
	}
	_client->setConnectionCallback(std::bind(&ClientHandler::onConnection, this, std::placeholders::_1));
	_client->setMessageCallback(std::bind(&ClientHandler::onMessage, this, std::placeholders::_1,
				std::placeholders::_2, std::placeholders::_3));
	_client->connect();
}

// }}}
// {{{ void ClientHandler::stop()

void ClientHandler::stop() {	
	_client->stop();
}

// }}}
// {{{ ClientHandler::~ClientHandler()

ClientHandler::~ClientHandler() {
	if (_client != nullptr) {
		delete _client;
		_client = nullptr;	
	}
}

// }}}
// {{{ void ClientHandler::onConnection()

void ClientHandler::onConnection(const TcpConnectionPtr& conn) {
	LOG_TRACE << conn->localAddress().toIpPort() << " -> "
			  << conn->peerAddress().toIpPort();	
	_connnection = std::move(conn);
	if (_cmdCache.readableBytes()) {
		uint32_t textlen = static_cast<uint32_t>(_cmdCache.readableBytes());
		_connnection->send(_cmdCache.peek(), textlen);
		LOG_DEBUG << textlen;
		_cmdCache.retrieveAll();
	}
}

// }}}
// {{{ void ClientHandler::onMessage()

void ClientHandler::onMessage(const TcpConnectionPtr& conn, evbuffer* evbuf, Timestamp reciveTime) {	
	(void)conn;
	LOG_DEBUG << "Recive time:" << reciveTime.toString();
	LOG_DEBUG << "Read buffer len:" << evbuffer_get_length(evbuf);
	
	if (_binary) {
		processDataBinary(evbuf);	
	} else {
		processDataAscii(evbuf);	
	}
}

// }}}
// {{{ bool ClientHandler::get()

bool ClientHandler::get(const std::string& key) {
	if (!_connnection || !_connnection->connected()) {
		if (_connnection != nullptr) {
			_connnection.reset(); 	
			_connnection = nullptr;
		}
		LOG_TRACE << "This connection is close.";
		return false;
	}

	if (_binary) {
		ProtocolBinaryRequestHeader header;			
		memset(&header, 0, sizeof(header));
		
		header.request.magic  = PROTOCOL_BINARY_REQ;
		header.request.opcode = PROTOCOL_BINARY_CMD_GET;
		header.request.keylen = hostToNetwork16(static_cast<uint16_t>(key.size()));
		header.request.bodylen = hostToNetwork32(static_cast<uint32_t>(key.size()));

		if (_connnection) {
			_connnection->send(header.bytes, sizeof(header.bytes));
			_connnection->send(key.c_str(), static_cast<uint32_t>(key.size()));
		} else { // 如果没有创建好连接，预先写到缓存中，等连接好在发送
			if (_cmdCache.readableBytes() > 4096) {
				LOG_TRACE << "This connection is not create, send buffer > 4096, message lost."; 	
				return false;
			} else {
				_cmdCache.append(header.bytes, sizeof(header.bytes));
				_cmdCache.append(key.c_str(), static_cast<uint32_t>(key.size()));
			}
		}
	} else {
		std::string cmd = "get " + key + "\r\n";		
		if (_connnection) {
			_connnection->send(cmd.c_str(), static_cast<uint32_t>(cmd.size()));
		} else { // 如果没有创建好连接，预先写到缓存中，等连接好在发送
			if (_cmdCache.readableBytes() > 4096) {
				LOG_TRACE << "This connection is not create, send buffer > 4096, message lost."; 	
				return false;
			} else {
				_cmdCache.append(cmd.c_str(), static_cast<uint32_t>(cmd.size()));
			}
		}
	}

	return true;
}

// }}}
// {{{ void ClientHandler::processDataBinary()

void ClientHandler::processDataBinary(evbuffer* evbuf) {
	while (1) {
		ssize_t len = evbuffer_get_length(evbuf);	
		ssize_t headerLen = static_cast<ssize_t>(sizeof(ProtocolBinaryResponseHeader));
		if (len < headerLen) {
			// 协议包数据不完整	
			LOG_TRACE << "Read response header len less " << headerLen << " real len:" << len;
			break;
		}

		ProtocolBinaryResponseHeader header;
		if (evbuffer_copyout(evbuf, &header, headerLen) != headerLen) {
			// 读取错误，关闭连接
			LOG_ERROR << "Read buffer error"; 
			return;
		}

		if (evbuffer_drain(evbuf, headerLen) == -1) {
			// 读取错误，关闭连接
			LOG_ERROR << "Read buffer error"; 
			break;
		}

		ssize_t extLen  = static_cast<ssize_t>(header.response.extlen);
		ssize_t keyLen  = static_cast<ssize_t>(networkToHost16(header.response.keylen));
		ssize_t bodyLen = static_cast<ssize_t>(networkToHost32(header.response.bodylen));
		ssize_t bodyTextLen = bodyLen - extLen - keyLen;
		char keyText[keyLen];
		char bodyText[bodyTextLen];

		LOG_TRACE << keyLen;
		if (bodyLen) {
			if (bodyLen > len - headerLen) {
				// 协议包数据不完整	
				LOG_TRACE << "Read response body len less " << bodyLen << " real len:" << len - headerLen - extLen;
				break;
			}

			if (extLen) {
				if (evbuffer_drain(evbuf, extLen) == -1) {
					// 读取错误，关闭连接
					LOG_ERROR << "Read buffer error"; 
					break;
				}
			}

			if (evbuffer_copyout(evbuf, keyText, keyLen) != keyLen) {
				// 读取错误，关闭连接
				LOG_TRACE << "Read response real len:" << len;
				LOG_ERROR << "Read buffer error"; 
				break;
			}

			if (evbuffer_copyout(evbuf, bodyText, bodyTextLen) != bodyTextLen) {
				// 读取错误，关闭连接
				LOG_TRACE << "Read response real len:" << len;
				LOG_ERROR << "Read buffer error"; 
				break;
			}

			if (evbuffer_drain(evbuf, bodyLen) == -1) {
				// 读取错误，关闭连接
				LOG_ERROR << "Read buffer error"; 
				break;
			}
		}

		uint8_t cmdOp = header.response.opcode;
		ProtocolBinaryCommand cmd = static_cast<ProtocolBinaryCommand>(cmdOp);

		if (cmd == PROTOCOL_BINARY_CMD_GET) {
			Buffer result;
			result.append(bodyText, bodyTextLen);
			_getCallback(std::string(keyText, keyLen), &result, this);
		}
	}
}

// }}}
// {{{ void ClientHandler::processDataAscii()

void ClientHandler::processDataAscii(evbuffer* evbuf) {
	size_t len;
	while (1) {
		if (_currentGetCmd == getHeaderState || _currentGetCmd == getEndState) {
			char* requestLine = evbuffer_readln(evbuf, &len, EVBUFFER_EOL_CRLF);
			if (requestLine == nullptr) {
				// 没有读完一行
				break;
			}
			char headerToken[] = "VALUE ";
			char endToken[] = "END";
			if (strncmp(headerToken, requestLine, strlen(headerToken)) == 0) { // get 消息头部
				std::vector<std::string> tokens = adbase::explode(std::string(requestLine, len), ' ', true);
				if (tokens.size() >= 4) { // key
					_resultBuffer.append(tokens[1]);
					_getDataSize = std::stoi(tokens[3]);
					LOG_TRACE << " Data size:" << _getDataSize;
				}
				_currentGetCmd = getDataState;
			} else if (strncmp(endToken, requestLine, strlen(endToken)) == 0 && len == 3) { // end
				_getCallback(_resultBuffer.retrieveAllAsString(), &_bodyBuffer, this);
				_resultBuffer.retrieveAll();
				_bodyBuffer.retrieveAll();
				_currentGetCmd = getHeaderState;
				_getDataSize = 0;
			}
			free(requestLine);
		} else {
			ssize_t len = evbuffer_get_length(evbuf);	
			ssize_t dataLen = static_cast<ssize_t>(_getDataSize);
			char dataText[dataLen];
			if (len < dataLen) {
				// 协议包数据不完整	
				LOG_TRACE << "Read response data len less " << dataLen << " real len:" << len;
				break;
			}
			if (evbuffer_copyout(evbuf, dataText, dataLen) != dataLen) {
				// 读取错误，关闭连接
				LOG_ERROR << "Read buffer error"; 
				return;
			}

			if (evbuffer_drain(evbuf, dataLen) == -1) {
				// 读取错误，关闭连接
				LOG_ERROR << "Read buffer error"; 
				break;
			}
			_bodyBuffer.append(dataText, dataLen);
			_currentGetCmd = getEndState;
		}
	}
}

// }}}
}
}
