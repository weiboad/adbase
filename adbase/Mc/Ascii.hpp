#if !defined ADBASE_MC_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_MC_ASCII_HPP_
#define ADBASE_MC_ASCII_HPP_

#include <adbase/Mc/Interface.hpp>
#include <adbase/Net.hpp>
#include <vector>

namespace adbase {

/**
 * @addtogroup mc adbase::Mc
 */
/*@{*/

namespace mc {
// {{{ AsciiCmd

typedef enum AsciiCmd {
	GET_CMD,
	GETS_CMD,
	SET_CMD,
	ADD_CMD,
	REPLACE_CMD,
	CAS_CMD,
	APPEND_CMD,
	PREPEND_CMD,
	DELETE_CMD,
	INCR_CMD,
	DECR_CMD,
	STATS_CMD,
	FLUSH_ALL_CMD,
	VERSION_CMD,
	QUIT_CMD,
	VERBOSITY_CMD,
	UNKNOWN_CMD
} AsciiCmd;

// }}}
class Ascii {
public:
	Ascii(Interface* interface);
	~Ascii();	
	void processData(const TcpConnectionPtr& conn, evbuffer* evbuf);

private:
	Interface* _interface;
	int processGets(const TcpConnectionPtr& conn, char* requestLine, size_t len);
	int processVersionCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len);
	int processStorageCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len);
	int processVerbosityCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len);
	int processDeleteCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len);
	int processCrementCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len);
	int processQuitCommand(const TcpConnectionPtr& conn);
	int processFlushCommand(const TcpConnectionPtr& conn, char* requestLine, size_t len);
	void sendCommandUsage(const TcpConnectionPtr& conn);
	void sendNotImplementedError(const TcpConnectionPtr& conn);
	AsciiCmd asciiToCmd(char *start, size_t len);	
	uint16_t parseKey(char **start);
};

}

/*@}*/

}

#endif
