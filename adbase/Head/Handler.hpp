#if !defined ADBASE_HEAD_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_HEAD_HANDLER_HPP_
#define ADBASE_HEAD_HANDLER_HPP_


#include <unordered_map>
#include <mutex>
#include <adbase/Utility.hpp>
#include <adbase/Net.hpp>
#include <adbase/Head/Interface.hpp>

namespace adbase {

/**
 * @addtogroup head adbase::Head
 */
/*@{*/

namespace head {
class Handler {
public:
	Handler(Interface *interface);
	~Handler();
	void onConnection(const TcpConnectionPtr& conn);
	void onMessage(const TcpConnectionPtr& conn, evbuffer* evbuf, Timestamp reciveTime);
	void onClose(const TcpConnectionPtr& conn);

private:
	Interface* _interface;
	mutable std::mutex _mut;
};	

}	

/*@}*/

}
#endif
