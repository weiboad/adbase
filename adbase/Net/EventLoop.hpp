#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_EVENTLOOP_HPP_
#define ADBASE_NET_EVENTLOOP_HPP_

#include <adbase/Net/Types.hpp>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

class EventLoop {
public:	
	EventLoop();
	~EventLoop();
	EventBasePtr& getBase();
	void start();
	void stop();
	void isStop(void*);

private:
	EventBasePtr _base;
	//std::vector<int> _pipefds;
	//struct event _pipeEvent;
	mutable std::mutex _mut;
	typedef enum baseStatus {
		RUNNING = 0,
		STOPING = 1,
		STOPED  = 2,
	} BaseStatus;
	BaseStatus _status;
	adbase::Timer* _timer = nullptr;
};

/*@}*/

}

#endif
