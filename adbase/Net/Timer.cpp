#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Metrics.hpp>

namespace adbase {
namespace detail {
// {{{ void timerHandler()

void timerHandler(int, short, void *arg) {
	TimerItem* item = reinterpret_cast<TimerItem*>(arg);		
	item->timerHandler();
}

// }}}
}
// {{{ Timer::Timer()

Timer::Timer(EventBasePtr& base) :
	_base(base),
	_timerSeq(0) {
}

// }}}
// {{{ Timer::~Timer()

Timer::~Timer() {
}

// }}}
// {{{  uint64_t Timer::runAfter()

uint64_t Timer::runAfter(int interval, const TimerCallback& cb, void* context) {
	_timerSeq++;
	TimerItemPtr item(new TimerItem(_timerSeq, _base, interval, cb, context, false));	
	item->setDelTimerCallback(std::bind(&Timer::cancel, this, std::placeholders::_1));
	item->start();
	_timerMap[_timerSeq] = std::move(item);
	return _timerSeq;
}

// }}}
// {{{  uint64_t Timer::runEvery()

uint64_t Timer::runEvery(int interval, const TimerCallback& cb, void* context) {
	_timerSeq++;
	TimerItemPtr item(new TimerItem(_timerSeq, _base, interval, cb, context, true));	
	item->setDelTimerCallback(std::bind(&Timer::cancel, this, std::placeholders::_1));
	item->start();
	_timerMap[_timerSeq] = std::move(item);
	return _timerSeq;
}

// }}}
// {{{ void Timer::cancel()

void Timer::cancel(uint64_t timerId) {
	if (_timerMap.find(timerId) != _timerMap.end()) {
		_timerMap.erase(timerId);	
	}	
}

// }}}
// {{{ void Timer::stop()

void Timer::stop() {
	std::vector<uint64_t> ids;
	for (auto &t : _timerMap) {
		_timerMap[t.first]->stop();
		ids.push_back(t.first);
	}
	for (auto &t : ids) {
		_timerMap.erase(t);	
	}
}

// }}}
// {{{ TimerItem::TimerItem()

TimerItem::TimerItem(uint64_t timerId, EventBasePtr& base, 
					 int interval, const TimerCallback& cb, void* context, bool isEvery) :
	_timerId(timerId),
	_base(base),
	_callback(cb),
	_context(context),
	_isEvery(isEvery) {
	_timeval.tv_sec  = interval / 1000;
	_timeval.tv_usec = (interval % 1000) * 1000;
	EventPtr eventptr(evtimer_new(_base.get(), &detail::timerHandler, this),
	                          std::ptr_fun(&event_free));
	_event = std::move(eventptr);
}

// }}}
// {{{ TimerItem::~TimerItem()

TimerItem::~TimerItem() {
	LOG_DEBUG << " Timer Id:" << _timerId << " dor";
}

// }}}
// {{{ void TimerItem::start()

void TimerItem::start() {
	evtimer_add(_event.get(), &_timeval);
}

// }}}
// {{{ void TimerItem::stop()

void TimerItem::stop() {
	if (_delTimer) {
		_delTimer(_timerId);	
	}
}

// }}}
// {{{ void TimerItem::timerHandler()

void TimerItem::timerHandler() {
	_callback(_context);	
	if (_isEvery) {
		evtimer_add(_event.get(), &_timeval);
	} else { // 如果一次性定时器则回收资源
		stop();
	}
}

// }}}
}
