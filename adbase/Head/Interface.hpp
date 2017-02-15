#if !defined ADBASE_HEAD_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_HEAD_INTERFACE_HPP_
#define ADBASE_HEAD_INTERFACE_HPP_

#include <functional>
#include <adbase/Head/Binary.hpp>

namespace adbase {

/**
 * @addtogroup head adbase::Head
 */
/*@{*/

class Buffer;
namespace head {

// {{{ Callbacks define

// ReadHandler
typedef std::function<ProtocolBinaryResponseStatus (ProtocolBinaryDataType datatype,
													const void* body,
													ssize_t bodylen,
													ProtocolBinaryDataType* resDataType,
													Buffer* data)> ReadHandler;


// }}}

class Interface {
public:
	Interface();
	~Interface();	
	void setReadHandler(const ReadHandler& readHandler);
	const ReadHandler& getReadHandler();
	
private:
	ReadHandler _readHandler;	
};

}

/*@}*/

}
#endif
