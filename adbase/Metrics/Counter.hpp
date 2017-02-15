#if !defined ADBASE_METRICS_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_METRICS_COUNTER_HPP_
#define ADBASE_METRICS_COUNTER_HPP_

#include <adbase/Metrics/Metrics.hpp>

namespace adbase {

/**
 * @addtogroup metrics adbase::Metrics
 */
/*@{*/

namespace metrics {

class Counter {
public:
	Counter(const std::string moduleName,
		   const std::string metricName,
		   Metrics* metrics,
		   adbase::Queue<DataItem>* queue);
	const std::string& getModuleName(); 
	const std::string& getMetricName(); 
	void add(int64_t val = 1);
	void dec(int64_t val = 1);
	~Counter();

private:
	std::string  _moduleName;
	std::string  _metricName;
	adbase::Queue<DataItem>* _queue;
};

}

/*@}*/

}

#endif
