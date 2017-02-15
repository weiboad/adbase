#if !defined ADBASE_MC_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_MC_CONTEXT_HPP_
#define ADBASE_MC_CONTEXT_HPP_

#include <adbase/Utility.hpp>
#include <adbase/Mc/Ascii.hpp>

namespace adbase {

/**
 * @addtogroup mc adbase::Mc
 */
/*@{*/

namespace mc {
class Context {			
public:
	Context();
	~Context();
	void setAsciiBuffer(const char* data, size_t len);
	const char* getAsciiBuffer(size_t& len);
	int getAsciiBufferSize();

	void setAsciiCmd(AsciiCmd cmd) {
		_asciiCurrentCmd = cmd;	
	}

	AsciiCmd getAsciiCmd() {
		return _asciiCurrentCmd;	
	}
	
private:
	AsciiCmd _asciiCurrentCmd; 	
	Buffer _asciiBuffer;
};
	
}	

/*@}*/

}
#endif
