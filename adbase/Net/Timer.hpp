#if !defined ADBASE_NET_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_NET_TIMER_HPP_
#define ADBASE_NET_TIMER_HPP_

#include <adbase/Net/Types.hpp>
#include <unordered_map>

namespace adbase {

/**
 * @addtogroup net adbase::Net
 */
/*@{*/

namespace detail {
void timerHandler(int, short evt, void *arg);
}

class TimerItem {
public:
	TimerItem(uint64_t timerId, EventBasePtr& base,
			  int interval, const TimerCallback& cb, void* context, bool isEvery = false);
	~TimerItem();
	void start();
	void stop();
	void timerHandler();

	void setDelTimerCallback(const DelTimerCallback& dc) {
		_delTimer = dc;
	}

private:
	uint64_t _timerId;
	EventBasePtr& _base;
	TimerCallback _callback;
	void* _context;
	bool _isEvery;
	struct timeval _timeval;	
	EventPtr _event;
	DelTimerCallback _delTimer;
};

typedef std::shared_ptr<TimerItem> TimerItemPtr;
// 非线程安全
class Timer {
public:
	Timer(EventBasePtr& base);	
	~Timer();
	uint64_t runEvery(int interval, const TimerCallback& cb, void* context);
	uint64_t runAfter(int interval, const TimerCallback& cb, void* context);
	void cancel(uint64_t timerId);
	void stop();
private:
	EventBasePtr& _base;
	uint64_t _timerSeq;
	typedef std::unordered_map<uint64_t, TimerItemPtr> TimerMap;
	TimerMap _timerMap;
};

/*@}*/

}

#endif
