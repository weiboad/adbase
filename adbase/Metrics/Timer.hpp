#if !defined ADBASE_METRICS_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_METRICS_TIMER_HPP_
#define ADBASE_METRICS_TIMER_HPP_

namespace adbase {

/**
 * @addtogroup metrics adbase::Metrics
 */
/*@{*/

namespace metrics {

class Timer {
public:
	Timer();
	void start();
	double stop();
	~Timer();

private:
	std::chrono::steady_clock::time_point _startTime;
};

}

/*@}*/

}

#endif
