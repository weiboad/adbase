#if !defined ADBASE_METRICS_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_METRICS_GAUGES_HPP_
#define ADBASE_METRICS_GAUGES_HPP_

namespace adbase {

/**
 * @addtogroup metrics adbase::Metrics
 */
/*@{*/

namespace metrics {

class Gauges {
public:
	Gauges(const std::string moduleName,
		   const std::string metricName,
		   uint32_t interval,
		   Metrics* metrics,
		   const GaugesDataCallback& func);
	uint32_t interval(); 
	const GaugesDataCallback& getCallback();
	const std::string& getModuleName(); 
	const std::string& getMetricName(); 
	~Gauges();

private:
	std::string  _moduleName;
	std::string  _metricName;
	GaugesDataCallback _func;
	uint32_t _interval;
};

}

/*@}*/

}

#endif
