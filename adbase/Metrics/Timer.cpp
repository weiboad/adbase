#include <adbase/Metrics.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace metrics {
// {{{ Timer::Timer()

Timer::Timer() {
	_startTime  = std::chrono::steady_clock::now(); 
}

// }}}
// {{{ void Timer::start()

void Timer::start() {
	_startTime  = std::chrono::steady_clock::now(); 
}

// }}}
// {{{ void Timer::stop()

double Timer::stop() {
	std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - _startTime);
	double time = static_cast<double>(time_span.count() * 1000);
	return time;
}

// }}}
// {{{ Timer::~Timer()

Timer::~Timer() {
}

// }}}
}
}
