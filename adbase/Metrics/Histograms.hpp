#if !defined ADBASE_METRICS_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_METRICS_HISTOGRAMS_HPP_
#define ADBASE_METRICS_HISTOGRAMS_HPP_

#include <adbase/Metrics/Metrics.hpp>

namespace adbase {

/**
 * @addtogroup metrics adbase::Metrics
 */
/*@{*/

namespace metrics {

class Histograms {
public:
	Histograms(const std::string moduleName,
		   const std::string metricName,
		   uint32_t interval,
		   Metrics* metrics,
		   adbase::Queue<DataItem>* queue);
	uint32_t interval(); 
	const std::string& getModuleName(); 
	const std::string& getMetricName(); 
	void update(double val);
	~Histograms();

private:
	std::string  _moduleName;
	std::string  _metricName;
	uint32_t _interval;
	adbase::Queue<DataItem>* _queue;
};

}

/*@}*/

}

#endif
