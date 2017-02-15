#if !defined ADBASE_MC_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_MC_HANDLER_HPP_
#define ADBASE_MC_HANDLER_HPP_

#include <unordered_map>
#include <mutex>
#include <adbase/Utility.hpp>
#include <adbase/Net.hpp>
#include <adbase/Mc/Interface.hpp>
#include <adbase/Mc/Context.hpp>

namespace adbase {

/**
 * @addtogroup mc adbase::Mc
 */
/*@{*/

namespace mc {
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
	std::unordered_map<std::string, Context*> _contexts;
};	

}	

/*@}*/

}
#endif
