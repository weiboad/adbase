#if !defined ADBASE_HEAD_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_HEAD_BINARY_HPP_
#define ADBASE_HEAD_BINARY_HPP_

#include <vector>
#include <adbase/Net.hpp>

namespace adbase {

/**
 * @addtogroup head adbase::Head
 */
/*@{*/

namespace head {
class Interface;
	// {{{ Magic

	/**
	 * Definition of the legal "magic" values used in a packet.
	 */
	typedef enum {
		PROTOCOL_BINARY_REQ = 0x90,
		PROTOCOL_BINARY_RES = 0x91
	} ProtocolBinaryMagic;		

	// }}}
	// {{{ Respondse Code

	/**
	 * Definition of the valid response status numbers. 
	 */
	typedef enum {
		PROTOCOL_BINARY_RESPONSE_SUCCESS = 0x00,
		PROTOCOL_BINARY_RESPONSE_ERROR = 0x01,
		PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND = 0x02,
		PROTOCOL_BINARY_RESPONSE_NOT_SUPPORTED = 0x03,
		PROTOCOL_BINARY_RESPONSE_EBUSY = 0x04,
	} ProtocolBinaryResponseStatus;	

	// }}}
	// {{{ DataType

    /**
     * Defintion of the different command opcodes.
     */
	typedef enum {
		PROTOCOL_BINARY_TYPE_ASCII = 0x00,
		PROTOCOL_BINARY_TYPE_JSON  = 0x01,
		PROTOCOL_BINARY_TYPE_GPB   = 0x02,
		PROTOCOL_BINARY_TYPE_RAW   = 0x03,
	} ProtocolBinaryDataType;

	// }}}
	// {{{ header

	/**
	 * Definition of the header structure for a request or repsonse packet.
	 * See section 2
	 */
	typedef union {
		struct {
			uint8_t magic;
			uint8_t datatype;
			uint8_t masterversion;
			uint8_t secondversion;
			uint16_t status;
			uint16_t opaque;
			uint32_t appkey;
			uint32_t bodylen;
			uint64_t reserved; // 保留段
		} head;
		uint8_t bytes[24];
	} ProtocolBinaryHeader;

	// }}}

class Binary {
public:
	Binary(Interface* interface);
	~Binary();		
	void processData(const TcpConnectionPtr& conn, evbuffer* evbuf);

private:
	Interface* _interface;
};

}	

/*@}*/

}

#endif
