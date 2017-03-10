#if !defined ADBASE_METRICS_HPP_
# error "Not allow include this header."
#endif

#ifndef ADBASE_METRICS_METERS_HPP_
#define ADBASE_METRICS_METERS_HPP_

#include <numeric>

namespace adbase {

/**
 * @addtogroup metrics adbase::Metrics
 */
/*@{*/

namespace metrics {

class Counter;
class Metrics;
class Meters {
public:
	Meters(const std::string moduleName,
		   const std::string metricName,
		   Metrics* metrics);
	const std::string& getModuleName();
	const std::string& getMetricName();
	void mark();
	int64_t getCounter();
	~Meters();

private:
	std::string  _moduleName;
	std::string  _metricName;
	Counter* _counter;
	Metrics* _metrics;
};

}

/*@}*/

}

#endif
