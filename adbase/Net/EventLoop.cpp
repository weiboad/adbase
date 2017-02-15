#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
// {{{ EventLoop::EventLoop()

EventLoop::EventLoop() {
	EventBasePtr tmp(event_base_new(), std::ptr_fun(&event_base_free));
	_base = std::move(tmp);
}

// }}}
// {{{ EventLoop::~EventLoop()

EventLoop::~EventLoop() {
}

// }}}
// {{{ void EventLoop::start()

void EventLoop::start() {
	{
		std::lock_guard<std::mutex> lk(_mut);
		_status = RUNNING;
	}
	_timer = new adbase::Timer(_base);
	_timer->runEvery(10, std::bind(&EventLoop::isStop, this, std::placeholders::_1), nullptr);
	event_base_loop(_base.get(), 0);
	{
		std::lock_guard<std::mutex> lk(_mut);
		_status = STOPED;
	}
}

// }}}
// {{{ void EventLoop::stop()

void EventLoop::stop() {
	{
		std::lock_guard<std::mutex> lk(_mut);
		_status = STOPING;
	}
	int i = 5;
	while(i >= 0) {
		{
			std::lock_guard<std::mutex> lk(_mut);
			if (_status == STOPED) {
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));	
		i--;
	}
	if (_timer != nullptr) {
		delete _timer;
	}
	LOG_ERROR << "Event loop stoped.";
}

// }}}
// {{{ void EventLoop::isStop()

void EventLoop::isStop(void*) {
	std::lock_guard<std::mutex> lk(_mut);
	if (_status == STOPING) {
		int breakret = event_base_loopbreak(_base.get());
		LOG_ERROR << "Event base break ret:" << breakret;
		int ret = event_base_loopexit(_base.get(), nullptr);
		LOG_ERROR << "Event base exit ret:" << ret;
	}
}

// }}}
// {{{ EventBasePtr& EventLoop::getBase();

EventBasePtr& EventLoop::getBase() {
	return _base;
}

// }}}
}

