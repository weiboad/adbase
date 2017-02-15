#if !defined ADBASE_MC_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_MC_BINARY_HPP_
#define ADBASE_MC_BINARY_HPP_

#include <vector>
#include <adbase/Net.hpp>

namespace adbase {

/**
 * @addtogroup mc adbase::Mc
 */
/*@{*/

namespace mc {
class Interface;
	// {{{ Magic

	/**
	 * Definition of the legal "magic" values used in a packet.
	 */
	typedef enum {
		PROTOCOL_BINARY_REQ = 0x80,
		PROTOCOL_BINARY_RES = 0x81
	} ProtocolBinaryMagic;		

	// }}}
	// {{{ Respondse Code

	/**
	 * Definition of the valid response status numbers. 
	 */
	typedef enum {
		PROTOCOL_BINARY_RESPONSE_SUCCESS = 0x00,
		PROTOCOL_BINARY_RESPONSE_KEY_ENOENT = 0x01,
		PROTOCOL_BINARY_RESPONSE_KEY_EEXISTS = 0x02,
		PROTOCOL_BINARY_RESPONSE_E2BIG = 0x03,
		PROTOCOL_BINARY_RESPONSE_EINVAL = 0x04,
		PROTOCOL_BINARY_RESPONSE_NOT_STORED = 0x05,
		PROTOCOL_BINARY_RESPONSE_DELTA_BADVAL = 0x06,
		PROTOCOL_BINARY_RESPONSE_NOT_MY_VBUCKET = 0x07,
		PROTOCOL_BINARY_RESPONSE_AUTH_ERROR = 0x20,
		PROTOCOL_BINARY_RESPONSE_AUTH_CONTINUE = 0x21,
		PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND = 0x81,
		PROTOCOL_BINARY_RESPONSE_ENOMEM = 0x82,
		PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED = 0x83,
		PROTOCOL_BINARY_RESPONSE_EINTERNAL = 0x84,
		PROTOCOL_BINARY_RESPONSE_EBUSY = 0x85,
		PROTOCOL_BINARY_RESPONSE_ETMPFAIL = 0x86
	} ProtocolBinaryResponseStatus;	

	// }}}
	// {{{ CMD

    /**
     * Defintion of the different command opcodes.
     */
	typedef enum {
		PROTOCOL_BINARY_CMD_GET = 0x00,
		PROTOCOL_BINARY_CMD_SET = 0x01,
		PROTOCOL_BINARY_CMD_ADD = 0x02,
		PROTOCOL_BINARY_CMD_REPLACE = 0x03,
		PROTOCOL_BINARY_CMD_DELETE = 0x04,
		PROTOCOL_BINARY_CMD_INCREMENT = 0x05,
		PROTOCOL_BINARY_CMD_DECREMENT = 0x06,
		PROTOCOL_BINARY_CMD_QUIT = 0x07,
		PROTOCOL_BINARY_CMD_FLUSH = 0x08,
		PROTOCOL_BINARY_CMD_GETQ = 0x09,
		PROTOCOL_BINARY_CMD_NOOP = 0x0a,
		PROTOCOL_BINARY_CMD_VERSION = 0x0b,
		PROTOCOL_BINARY_CMD_GETK = 0x0c,
		PROTOCOL_BINARY_CMD_GETKQ = 0x0d,
		PROTOCOL_BINARY_CMD_APPEND = 0x0e,
		PROTOCOL_BINARY_CMD_PREPEND = 0x0f,
		PROTOCOL_BINARY_CMD_STAT = 0x10,
		PROTOCOL_BINARY_CMD_SETQ = 0x11,
		PROTOCOL_BINARY_CMD_ADDQ = 0x12,
		PROTOCOL_BINARY_CMD_REPLACEQ = 0x13,
		PROTOCOL_BINARY_CMD_DELETEQ = 0x14,
		PROTOCOL_BINARY_CMD_INCREMENTQ = 0x15,
		PROTOCOL_BINARY_CMD_DECREMENTQ = 0x16,
		PROTOCOL_BINARY_CMD_QUITQ = 0x17,
		PROTOCOL_BINARY_CMD_FLUSHQ = 0x18,
		PROTOCOL_BINARY_CMD_APPENDQ = 0x19,
		PROTOCOL_BINARY_CMD_PREPENDQ = 0x1a,
		PROTOCOL_BINARY_CMD_VERBOSITY = 0x1b,
		PROTOCOL_BINARY_CMD_TOUCH = 0x1c,
		PROTOCOL_BINARY_CMD_GAT = 0x1d,
		PROTOCOL_BINARY_CMD_GATQ = 0x1e,
		PROTOCOL_BINARY_CMD_GATK = 0x23,
		PROTOCOL_BINARY_CMD_GATKQ = 0x24,
		PROTOCOL_BINARY_CMD_SASL_LIST_MECHS = 0x20,
		PROTOCOL_BINARY_CMD_SASL_AUTH = 0x21,
		PROTOCOL_BINARY_CMD_SASL_STEP = 0x22,

		/* These commands are used for range operations and exist within
		 * this header for use in other projects.  Range operations are
		 * not expected to be implemented in the memcached server itself.
		 */
		PROTOCOL_BINARY_CMD_RGET      = 0x30,
		PROTOCOL_BINARY_CMD_RSET      = 0x31,
		PROTOCOL_BINARY_CMD_RSETQ     = 0x32,
		PROTOCOL_BINARY_CMD_RAPPEND   = 0x33,
		PROTOCOL_BINARY_CMD_RAPPENDQ  = 0x34,
		PROTOCOL_BINARY_CMD_RPREPEND  = 0x35,
		PROTOCOL_BINARY_CMD_RPREPENDQ = 0x36,
		PROTOCOL_BINARY_CMD_RDELETE   = 0x37,
		PROTOCOL_BINARY_CMD_RDELETEQ  = 0x38,
		PROTOCOL_BINARY_CMD_RINCR     = 0x39,
		PROTOCOL_BINARY_CMD_RINCRQ    = 0x3a,
		PROTOCOL_BINARY_CMD_RDECR     = 0x3b,
		PROTOCOL_BINARY_CMD_RDECRQ    = 0x3c,
		/* End Range operations */

		/* VBucket commands */
		PROTOCOL_BINARY_CMD_SET_VBUCKET = 0x3d,
		PROTOCOL_BINARY_CMD_GET_VBUCKET = 0x3e,
		PROTOCOL_BINARY_CMD_DEL_VBUCKET = 0x3f,
		/* End VBucket commands */

		/* TAP commands */
		PROTOCOL_BINARY_CMD_TAP_CONNECT = 0x40,
		PROTOCOL_BINARY_CMD_TAP_MUTATION = 0x41,
		PROTOCOL_BINARY_CMD_TAP_DELETE = 0x42,
		PROTOCOL_BINARY_CMD_TAP_FLUSH = 0x43,
		PROTOCOL_BINARY_CMD_TAP_OPAQUE = 0x44,
		PROTOCOL_BINARY_CMD_TAP_VBUCKET_SET = 0x45,
		PROTOCOL_BINARY_CMD_TAP_CHECKPOINT_START = 0x46,
		PROTOCOL_BINARY_CMD_TAP_CHECKPOINT_END = 0x47,
		/* End TAP */

		PROTOCOL_BINARY_CMD_LAST_RESERVED = 0xef,

		/* Scrub the data */
		PROTOCOL_BINARY_CMD_SCRUB = 0xf0
	} ProtocolBinaryCommand;

	// }}}
	// {{{ Data type

	/**
	 * Definition of the data types in the packet 
	 */
	typedef enum {
		PROTOCOL_BINARY_RAW_BYTES = 0x00
	} ProtocolBinaryDatatypes;

	// }}}
	// {{{ Request header

	/**
	 * Definition of the header structure for a request packet.
	 * See section 2
	 */
	typedef union {
		struct {
			uint8_t magic;
			uint8_t opcode;
			uint16_t keylen;
			uint8_t extlen;
			uint8_t datatype;
			uint16_t vbucket;
			uint32_t bodylen;
			uint32_t opaque;
			uint64_t cas;
		} request;
		uint8_t bytes[24];
	} ProtocolBinaryRequestHeader;

	// }}}
// {{{ Request

	/**
     * Definition of the packet used by set, add and replace
     * See section 4
     */
	typedef struct {
		uint32_t flags;
		uint32_t expiration;
	} ProtocolBinaryRequestSetExtras;
	typedef ProtocolBinaryRequestSetExtras ProtocolBinaryRequestAddExtras;	
	typedef ProtocolBinaryRequestSetExtras ProtocolBinaryRequestReplaceExtras;	

	/**
	 * Definition of the structure used by the increment and decrement
	 * command.
	 * See section 4
	 */
	typedef struct {
		uint64_t delta;
		uint64_t initial;
		uint32_t expiration;
	} ProtocolBinaryRequestIncrExtras;
	typedef ProtocolBinaryRequestIncrExtras ProtocolBinaryRequestDecrExtras;

	
    /**
     * Definition of the packet used by the flush command
     * See section 4
     * Please note that the expiration field is optional, so remember to see
     * check the header.bodysize to see if it is present.
     */
	typedef struct {
		uint32_t expiration;
	} ProtocolBinaryRequestFlushExtras;

// }}}
	// {{{ Respondse header

	/**
	 * Definition of the header structure for a response packet.
	 * See section 2
	 */
	typedef union {
		struct {
			uint8_t magic;
			uint8_t opcode;
			uint16_t keylen;
			uint8_t extlen;
			uint8_t datatype;
			uint16_t status;
			uint32_t bodylen;
			uint32_t opaque;
			uint64_t cas;
		} response;
		uint8_t bytes[24];
	} ProtocolBinaryResponseHeader;

	// }}}
// {{{ Respondse

	/**
	 * Definition of a response-packet containing no extras
	 */
	typedef union {
		struct {
			ProtocolBinaryResponseHeader header;
		} message;
		uint8_t bytes[sizeof(ProtocolBinaryResponseHeader)];
	} ProtocolBinaryResponseNoExtras;

	/**
	 * Definition of the packet returned from a successful get, getq, getk and
	 * getkq.
	 * See section 4
	 */
	typedef union {
		struct {
			ProtocolBinaryResponseHeader header;
			struct {
				uint32_t flags;
			} body;
		} message;
		uint8_t bytes[sizeof(ProtocolBinaryResponseHeader) + 4];
	} ProtocolBinaryResponseGet;

	/**
	 * Definition of the response from an incr or decr command
	 * command.
	 * See section 4
	 */
	typedef union {
		struct {
			ProtocolBinaryResponseHeader header;
			struct {
				uint64_t value;
			} body;
		} message;
		uint8_t bytes[sizeof(ProtocolBinaryResponseHeader) + 8];
	} ProtocolBinaryResponseIncr;
	typedef ProtocolBinaryResponseIncr ProtocolBinaryResponseDecr;
	
// }}}

class Binary {
public:
	Binary(Interface* interface);
	~Binary();		
	void processData(const TcpConnectionPtr& conn, evbuffer* evbuf);

private:
	Interface* _interface;
	void printCmd(ProtocolBinaryCommand cmd);
	ProtocolBinaryResponseStatus processVersion(const TcpConnectionPtr& conn,
												ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processSet(const TcpConnectionPtr& conn,
											ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processGet(const TcpConnectionPtr& conn,
											ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processNoop(const TcpConnectionPtr& conn,
											 ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processAdd(const TcpConnectionPtr& conn,
											ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processAppend(const TcpConnectionPtr& conn,
											   ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processPrepend(const TcpConnectionPtr& conn,
											    ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processReplace(const TcpConnectionPtr& conn,
											    ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processDecrement(const TcpConnectionPtr& conn,
												  ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processIncrement(const TcpConnectionPtr& conn,
												  ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processDelete(const TcpConnectionPtr& conn,
											   ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processFlush(const TcpConnectionPtr& conn,
											  ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processStat(const TcpConnectionPtr& conn,
											 ProtocolBinaryRequestHeader header, const char *buffer);
	ProtocolBinaryResponseStatus processQuit(const TcpConnectionPtr& conn,
											 ProtocolBinaryRequestHeader header, const char *buffer);
};

}

/*@}*/

}

#endif
