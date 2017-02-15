#include <adbase/Mc.hpp>
#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace mc {
// {{{ Ascii::Ascii()

Ascii::Ascii(Interface* interface) :
    _interface(interface) {
}

// }}}
// {{{ Ascii::~Ascii()

Ascii::~Ascii() {
}

// }}}
// {{{ void Ascii::processData()

void Ascii::processData(const TcpConnectionPtr& conn, evbuffer* evbuf) {
    size_t len;
    while (1) {
        char* requestLine = evbuffer_readln(evbuf, &len, EVBUFFER_EOL_CRLF);
        if (requestLine == nullptr) {
            // 没有读完一行
            break;
        }

        AsciiCmd cmd;
        Context *context = static_cast<Context*>(conn->getContext());
		LOG_DEBUG << "context size:" << context->getAsciiBufferSize();
        if (context->getAsciiBufferSize()) { // 复合命令数据包到达
            cmd = context->getAsciiCmd();
        } else {
            cmd = asciiToCmd(requestLine, len);
            context->setAsciiCmd(cmd);
            PreExecute preExecute = _interface->getPreExecute();
            if (preExecute) {
                preExecute();
            }
        }

        if (cmd == GET_CMD || cmd == GETS_CMD) {
            if (_interface->getGetHandler()) {
                processGets(conn, requestLine, len);
            } else {
                std::string errorStr = "SERVER_ERROR: Command not implemented";
                LOG_DEBUG << errorStr;
                conn->send(errorStr);
                conn->send("\r\n", 2);
            }
        } else {
            switch (cmd) {
                case SET_CMD:
                case ADD_CMD:
                case REPLACE_CMD:
                case APPEND_CMD:
                case PREPEND_CMD:
                    processStorageCommand(conn, requestLine, len);
                    break;
                case VERSION_CMD:
                    processVersionCommand(conn, requestLine, len);
                    break;
                case VERBOSITY_CMD:
                    processVerbosityCommand(conn, requestLine, len);
                    break;
                case DELETE_CMD:
                    processDeleteCommand(conn, requestLine, len);
                    break;
                case INCR_CMD:
                case DECR_CMD:
                    processCrementCommand(conn, requestLine, len);
                    break;
                case QUIT_CMD:
                    processQuitCommand(conn);
                    break;
                case FLUSH_ALL_CMD:
                    processFlushCommand(conn, requestLine, len);
                    break;
                case UNKNOWN_CMD:
                    sendCommandUsage(conn);
                    break;
                case CAS_CMD:
                case STATS_CMD:
                    sendNotImplementedError(conn);
                    break;
                default:
                    LOG_FATAL << "Impossible";
            }
        }
        free(requestLine);
    }
}

// }}}
// {{{  AsciiCmd Ascii::asciiToCmd()

AsciiCmd Ascii::asciiToCmd(char *start, size_t len) {
	LOG_DEBUG << "Ascii cmd length:" << len;
	LOG_DEBUG << "Ascii cmd length:" << start;
    struct {
        const char* cmd;
        size_t len;
        AsciiCmd cc;
    } commands[] =  {
        { .cmd= "get", .len= 3, .cc= GET_CMD },
        { .cmd= "gets", .len= 4, .cc= GETS_CMD },
        { .cmd= "set", .len= 3, .cc= SET_CMD },
        { .cmd= "add", .len= 3, .cc= ADD_CMD },
        { .cmd= "replace", .len= 7, .cc= REPLACE_CMD },
        { .cmd= "cas", .len= 3, .cc= CAS_CMD },
        { .cmd= "append", .len= 6, .cc= APPEND_CMD },
        { .cmd= "prepend", .len= 7, .cc= PREPEND_CMD },
        { .cmd= "delete_object", .len= 6, .cc= DELETE_CMD },
        { .cmd= "incr", .len= 4, .cc= INCR_CMD },
        { .cmd= "decr", .len= 4, .cc= DECR_CMD },
        { .cmd= "stats", .len= 5, .cc= STATS_CMD },
        { .cmd= "flush_all", .len= 9, .cc= FLUSH_ALL_CMD },
        { .cmd= "version", .len= 7, .cc= VERSION_CMD },
        { .cmd= "quit", .len= 4, .cc= QUIT_CMD },
        { .cmd= "verbosity", .len= 9, .cc= VERBOSITY_CMD },
        { .cmd= NULL, .len= 0, .cc= UNKNOWN_CMD }
    };

    int x = -1;
    while (commands[++x].len > 0) {
        if (len < commands[x].len) {
            continue;
        }

        if (strncmp(start, commands[x].cmd, commands[x].len) != 0) {
            continue;
        }

        if (len == commands[x].len || isspace(*(start + commands[x].len))) {
            return commands[x].cc;
        }
    }

    return UNKNOWN_CMD;
}

// }}}
// {{{ void Ascii::sendCommandUsage()

void Ascii::sendCommandUsage(const TcpConnectionPtr& conn) {
    const char *errmsg[]= {
        [GET_CMD]= "CLIENT_ERROR: Syntax error: get <key>*\r\n",
        [GETS_CMD]= "CLIENT_ERROR: Syntax error: gets <key>*\r\n",
        [SET_CMD]= "CLIENT_ERROR: Syntax error: set <key> <flags> <exptime> <bytes> [noreply]\r\n",
        [ADD_CMD]= "CLIENT_ERROR: Syntax error: add <key> <flags> <exptime> <bytes> [noreply]\r\n",
        [REPLACE_CMD]= "CLIENT_ERROR: Syntax error: replace <key> <flags> <exptime> <bytes> [noreply]\r\n",
        [CAS_CMD]= "CLIENT_ERROR: Syntax error: cas <key> <flags> <exptime> <bytes> <casid> [noreply]\r\n",
        [APPEND_CMD]= "CLIENT_ERROR: Syntax error: append <key> <flags> <exptime> <bytes> [noreply]\r\n",
        [PREPEND_CMD]= "CLIENT_ERROR: Syntax error: prepend <key> <flags> <exptime> <bytes> [noreply]\r\n",
        [DELETE_CMD]= "CLIENT_ERROR: Syntax error: delete_object <key> [noreply]\r\n",
        [INCR_CMD]= "CLIENT_ERROR: Syntax error: incr <key> <value> [noreply]\r\n",
        [DECR_CMD]= "CLIENT_ERROR: Syntax error: decr <key> <value> [noreply]\r\n",
        [STATS_CMD]= "CLIENT_ERROR: Syntax error: stats [key]\r\n",
        [FLUSH_ALL_CMD]= "CLIENT_ERROR: Syntax error: flush_all [timeout] [noreply]\r\n",
        [VERSION_CMD]= "CLIENT_ERROR: Syntax error: version\r\n",
        [QUIT_CMD]="CLIENT_ERROR: Syntax error: quit\r\n",

        [VERBOSITY_CMD]= "CLIENT_ERROR: Syntax error: verbosity <num>\r\n",
        [UNKNOWN_CMD]= "CLIENT_ERROR: Unknown command\r\n",
    };

    Context *context = static_cast<Context*>(conn->getContext());
    AsciiCmd cmd = context->getAsciiCmd();
    conn->send(errmsg[cmd], static_cast<int>(strlen(errmsg[cmd])));
}

// }}}
// {{{ void Ascii::sendNotImplementedError()

void Ascii::sendNotImplementedError(const TcpConnectionPtr& conn) {
    std::string errorStr = "SERVER_ERROR: callback not implemented";
    LOG_DEBUG << errorStr;
    errorStr += "\r\n";
    conn->send(errorStr);
}

// }}}
// {{{ uint16_t Ascii::parseKey()

uint16_t Ascii::parseKey(char **start) {
    uint16_t len = 0;
    char *c = *start;
    // 跳过空格
    while (isspace(*c)) {
        ++c;
    }

    *start = c;

    while (*c != '\0' && !isspace(*c) && !iscntrl(*c)) {
        ++c;
        ++len;
    }

    if (len == 0 || len > 240 || (*c != '\0' && *c != '\r' && iscntrl(*c))) {
        return 0;
    }

    return len;
}

// }}}
// {{{ int Ascii::processGets()

int Ascii::processGets(const TcpConnectionPtr& conn, char* requestLine, size_t len) {
    char *key = requestLine;
    char *end = key + len;

    if (len <= 4) {
        sendCommandUsage(conn);
        return -1;
    }

    Context *context = static_cast<Context*>(conn->getContext());
    AsciiCmd cmd = context->getAsciiCmd();
    key += (cmd == GETS_CMD) ? 5 : 4;

    int numKeys = 0;
    while (key < end) {
        uint16_t nkey = parseKey(&key);
        if (nkey == 0) { // 非法的key
            break;
        }

        GetHandler handler = _interface->getGetHandler();
        Buffer buffer;
        handler(key, nkey, &buffer);
        std::string header = "VALUE " + std::string(key, nkey) + " 0 " + std::to_string(buffer.readableBytes());
        if (cmd == GETS_CMD) {
            header += " 0";
        }
        header += "\r\n";
        conn->send(header);
        buffer.append("\r\n", 2);
        conn->send(buffer.peek(), static_cast<int>(buffer.readableBytes()));
        buffer.retrieve(buffer.readableBytes());
        key += nkey;
        ++numKeys;
    }

    if (numKeys == 0) {
        sendCommandUsage(conn);
        return -1;
    } else {
        conn->send("END\r\n", 5);
        return 0;
    }
}

// }}}
// {{{ int Ascii::processVersionCommand()

int Ascii::processVersionCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len) {
    std::vector<std::string> tokens = adbase::explode(adbase::trim(std::string(requestLine, len)), ' ', true);
    if (tokens.size() != 1) {
        sendCommandUsage(conn);
        return -1;
    }

    VersionHandler version = _interface->getVersionHandler();
    if (version) {
        Buffer buffer;
        version(&buffer);
        std::string versionData = "VERSION " + buffer.retrieveAllAsString() + "\r\n";
        conn->send(versionData);
        return 0;
    } else {
        sendNotImplementedError(conn);
        return -1;
    }
}

// }}}
// {{{ int Ascii::processVerbosityCommand()

int Ascii::processVerbosityCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len) {
    std::vector<std::string> tokens = adbase::explode(adbase::trim(std::string(requestLine, len)), ' ', true);
    if (tokens.size() != 2) {
        sendCommandUsage(conn);
        return -1;
    }

    uint32_t flags= static_cast<uint32_t>(strtoul(tokens[1].c_str(), nullptr, 10));
    if (errno != 0) {
        sendCommandUsage(conn);
        return -1;
    }

    VerbosityHandler verbosity = _interface->getVerbosityHandler();
    if (verbosity) {
        verbosity(flags);
        conn->send("OK\r\n", 4);
        return 0;
    } else {
        sendNotImplementedError(conn);
        return -1;
    }
}

// }}}
// {{{ int Ascii::processStorageCommand()

int Ascii::processStorageCommand(const TcpConnectionPtr& conn, char* data, size_t len) {
    Context *context = static_cast<Context*>(conn->getContext());
    if (!context->getAsciiBufferSize()) { // 操作头信息包到达，暂存起来等待 set 数据包到达
        std::vector<std::string> tokens = adbase::explode(adbase::trim(std::string(data, len)), ' ', true);
        if (tokens.size() != 5) {
            sendCommandUsage(conn);
            return -1;
        }
        context->setAsciiBuffer(data, len);
        return 0;
    }

    size_t headerLen = 0;
    const char* header = context->getAsciiBuffer(headerLen);
    std::vector<std::string> tokens = adbase::explode(adbase::trim(std::string(header, headerLen)), ' ', true);
    if (tokens.size() != 5) {
        sendCommandUsage(conn);
        return -1;
    }

    char *key = const_cast<char*>(tokens[1].c_str());
    uint16_t nkey = parseKey(&key);
    if (nkey == 0) {
        std::string err = "CLIENT_ERROR: bad key\r\n";
        conn->send(err);
        return -1;
    }

    errno = 0;
    uint32_t flags= static_cast<uint32_t>(strtoul(tokens[2].c_str(), nullptr, 10));
    if (errno != 0) {
        sendCommandUsage(conn);
        return -1;
    }

     uint32_t timeout= static_cast<uint32_t>(strtoul(tokens[3].c_str(), nullptr, 10));
    if (errno != 0) {
        sendCommandUsage(conn);
        return -1;
    }

      unsigned long nbytes= strtoul(tokens[4].c_str(), nullptr, 10);
    if (errno != 0) {
        sendCommandUsage(conn);
        return -1;
    }

    if (nbytes != len) {
        sendCommandUsage(conn);
        return -1;
    }

    // {{{ switch process stored cmd

    uint64_t result_cas = 0;
    uint64_t cas = 0;
    ProtocolBinaryResponseStatus rval = PROTOCOL_BINARY_RESPONSE_SUCCESS;
    SetHandler setHandler = _interface->getSetHandler();
    ReplaceHander replaceHandler = _interface->getReplaceHandler();
    AddHandler addHandler = _interface->getAddHandler();
    AppendHandler appendHandler = _interface->getAppendHandler();
    PrependHandler prependHandler = _interface->getPrependHandler();

    switch (context->getAsciiCmd()) {
        case SET_CMD:
            if (setHandler) {
                rval = setHandler(key,
                                  static_cast<uint16_t>(nkey), data,
                                  static_cast<uint32_t>(len),
                                  flags, timeout, cas, &result_cas);
            } else {
                sendNotImplementedError(conn);
                return -1;
            }
            break;
        case ADD_CMD:
            if (addHandler) {
                rval = addHandler(key,
                                  static_cast<uint16_t>(nkey), data,
                                  static_cast<uint32_t>(len),
                                  flags, timeout, &result_cas);
            } else {
                sendNotImplementedError(conn);
                return -1;
            }
            break;
        case REPLACE_CMD:
            if (replaceHandler) {
                rval = replaceHandler(key,
                                      static_cast<uint16_t>(nkey), data,
                                      static_cast<uint32_t>(len),
                                      flags, timeout, cas, &result_cas);
            } else {
                sendNotImplementedError(conn);
                return -1;
            }
            break;
        case APPEND_CMD:
            if (appendHandler) {
                rval = appendHandler(key,
                                     static_cast<uint16_t>(nkey), data,
                                     static_cast<uint32_t>(len),
                                     cas, &result_cas);
            } else {
                sendNotImplementedError(conn);
                return -1;
            }
            break;
        case PREPEND_CMD:
            if (prependHandler) {
                rval = prependHandler(key,
                                     static_cast<uint16_t>(nkey), data,
                                     static_cast<uint32_t>(len),
                                     cas, &result_cas);
            } else {
                sendNotImplementedError(conn);
                return -1;
            }
            break;
        default:
            LOG_FATAL << "Impossible";
    }

    // }}}

    if (rval == PROTOCOL_BINARY_RESPONSE_SUCCESS) {
        conn->send("STORED\r\n", 8);
    } else {
        conn->send("NOT_STORED\r\n", 12);
    }
    return 0;
}

// }}}
// {{{ int Ascii::processDeleteCommand()

int Ascii::processDeleteCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len) {
    std::vector<std::string> tokens = adbase::explode(adbase::trim(std::string(requestLine, len)), ' ', true);
    if (tokens.size() != 2) {
        sendCommandUsage(conn);
        return -1;
    }

    uint16_t nkey;
    char* key = const_cast<char*>(tokens[1].c_str());
    if ((nkey = parseKey(&key)) == 0) {
        sendCommandUsage(conn);
        return -1;
    }

    DeleteHandler deleteHandler = _interface->getDeleteHandler();
    if (deleteHandler) {
        ProtocolBinaryResponseStatus rval = deleteHandler(key, nkey, 0);
        std::string response = "";
        if (rval == PROTOCOL_BINARY_RESPONSE_SUCCESS) {
            response = "DELETED\r\n";
        } else if (rval == PROTOCOL_BINARY_RESPONSE_KEY_ENOENT) {
            response = "NOT_FOUND\r\n";
        } else {
            response = "SERVER_ERROR: delete_object failed " + std::to_string(rval) + "\r\n";
        }
        conn->send(response);
        return 0;
    } else {
        sendNotImplementedError(conn);
        return -1;
    }
}

// }}}
// {{{ int Ascii::processCrementCommand()

int Ascii::processCrementCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len) {
    std::vector<std::string> tokens = adbase::explode(adbase::trim(std::string(requestLine, len)), ' ', true);
    if (tokens.size() != 3) {
        sendCommandUsage(conn);
        return -1;
    }

    uint16_t nkey;
    char* key = const_cast<char*>(tokens[1].c_str());
    if ((nkey = parseKey(&key)) == 0) {
        sendCommandUsage(conn);
        return -1;
    }

    uint64_t cas;
    uint64_t result = 0;
    errno = 0;
    uint64_t delta= static_cast<uint64_t>(strtoull(tokens[2].c_str(), nullptr, 10));
    if (errno != 0) {
        sendCommandUsage(conn);
        return -1;
    }

    ProtocolBinaryResponseStatus rval;
    Context *context = static_cast<Context*>(conn->getContext());

    DecrementHandler decrementHandler = _interface->getDecrementHandler();
    if (context->getAsciiCmd() == INCR_CMD) {
        IncrementHandler incrementHandler = _interface->getIncrementHandler();
        if (incrementHandler) {
            rval = incrementHandler(key, nkey, delta, 0, 0, &result, &cas);
        } else {
            sendNotImplementedError(conn);
            return -1;
        }
    } else {
        DecrementHandler decrementHandler = _interface->getDecrementHandler();
        if (decrementHandler) {
            rval = decrementHandler(key, nkey, delta, 0, 0, &result, &cas);
        } else {
            sendNotImplementedError(conn);
            return -1;
        }
    }

    std::string response;
    if (rval == PROTOCOL_BINARY_RESPONSE_SUCCESS) {
        response = std::to_string(result) + "\r\n";
    } else {
        response = "NOT_FOUND\r\n";
    }
    conn->send(response);
    return 0;
}

// }}}
// {{{ int Ascii::processQuitCommand()

int Ascii::processQuitCommand(const TcpConnectionPtr& conn) {
    QuitHandler quitHandler = _interface->getQuitHandler();
    if (quitHandler) {
        quitHandler();
    }

    conn->shutdown();
    return 0;
}

// }}}
// {{{ int Ascii::processFlushCommand()

int Ascii::processFlushCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len) {
    std::vector<std::string> tokens = adbase::explode(adbase::trim(std::string(requestLine, len)), ' ', true);
    if (tokens.size() > 2) {
        sendCommandUsage(conn);
        return -1;
    }

    uint32_t timeout = 0;
    if (tokens.size() == 2) {
        errno = 0;
        timeout = static_cast<uint32_t>(strtoul(tokens[1].c_str(), nullptr, 10));
        if (errno != 0) {
            sendCommandUsage(conn);
            return -1;
        }
    }

    FlushHandler flushHandler = _interface->getFlushHandler();
    if (flushHandler) {
        ProtocolBinaryResponseStatus rval;
        rval = flushHandler(timeout);
        std::string response;
        if (rval == PROTOCOL_BINARY_RESPONSE_SUCCESS) {
            response = "OK\r\n";
        } else {
            response = "SERVER_ERROR: internal error\r\n";
        }
        conn->send(response);
        return 0;
    } else {
        sendNotImplementedError(conn);
        return -1;
    }
}

// }}}
}
}
