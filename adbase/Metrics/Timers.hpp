#if !defined ADBASE_METRICS_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_METRICS_TIMERS_HPP_
#define ADBASE_METRICS_TIMERS_HPP_

namespace adbase {

/**
 * @addtogroup metrics adbase::Metrics
 */
/*@{*/

namespace metrics {

class Meters;
class Histograms;
class Metrics;
class Timers {
public:
	Timers(const std::string moduleName,
		   const std::string metricName,
		   Metrics* metrics,
		   uint32_t interval);
	const std::string& getModuleName(); 
	const std::string& getMetricName(); 
	void start();
	void setTimer(double time);
	Meters* getMeters();
	Histograms* getHistograms();
	~Timers();

private:
	std::string  _moduleName;
	std::string  _metricName;
	Meters* _meters;
	Metrics* _metrics;
	Histograms* _histograms;
	std::chrono::steady_clock::time_point _startTime;
};

}

/*@}*/

}

#endif
