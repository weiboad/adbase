#include "Timer.hpp"
#include "App.hpp"

// {{{ macros

#define ADD_AFTER_TIMER(interval, name) do { \
	uint64_t timerId##name = _timer->runAfter((interval),\
					 std::bind(&Timer:: name, this, std::placeholders::_1), nullptr);\
	_timerIds.push_back(timerId##name);\
} while (0);
#define ADD_EVERY_TIMER(interval, name) do { \
	uint64_t timerId##name = _timer->runEvery((interval),\
					 std::bind(&Timer:: name, this, std::placeholders::_1), nullptr);\
	_timerIds.push_back(timerId##name);\
} while (0);

// }}}
// {{{ Timer::Timer()

Timer::Timer(TimerContext* context) :
	_context(context) {
	_configure = _context->config;
	_timer = new adbase::Timer(_context->mainEventBase);
}

// }}}
// {{{ adbase::Timer* Timer::getTimer() 

adbase::Timer* Timer::getTimer() {
	return _timer;
}

// }}}
// {{{ Timer::~Timer()

Timer::~Timer() {
	for (auto &t : _timerIds) {
		_timer->cancel(t);	
	}
	delete _timer;
}

// }}}
// {{{ void Timer::init()

void Timer::init() {
	/// 间隔一段时间执行一次
	//ADD_EVERY_TIMER(1 * 1000, one);
	/// 一段时间后仅执行一次
	//ADD_AFTER_TIMER(1 * 1000, one);
	//@IF @timer
	//@FOR @timers
	ADD_EVERY_TIMER(_configure->interval@REPLACE0|ucfirst@, @REPLACE0@);
	//@ENDFOR
	//@ENDIF
}

// }}}
//@IF @timer
//@FOR @timers
// {{{ void Timer::@REPLACE0@()

void Timer::@REPLACE0@(void*) {
	LOG_INFO << "Timer " << "@REPLACE0@";
}

// }}}
//@ENDFOR
//@ENDIF
