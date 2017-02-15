#include <adbase/Metrics.hpp>

namespace adbase {
namespace metrics {
// {{{ Gauges::Gauges()

Gauges::Gauges(const std::string moduleName, const std::string metricName, uint32_t interval, 
			   Metrics* metrics, const GaugesDataCallback& func) :
	_moduleName(moduleName),
	_metricName(metricName),
	_func(func),
	_interval(interval) {
	(void)metrics;
}

// }}}
// {{{ uint32_t Gauges::interval()

uint32_t Gauges::interval() {
	return _interval;
}

// }}}
// {{{ const GaugesDataCallback& Gauges::getCallback()

const GaugesDataCallback& Gauges::getCallback() {
	return _func;
}

// }}}
// {{{ const std::string& Gauges::getModuleName()

const std::string& Gauges::getModuleName() {
	return _moduleName;
}

// }}}
// {{{ const std::string& Gauges::getMetricName()

const std::string& Gauges::getMetricName() {
	return _metricName;
}

// }}}
// {{{ Gauges::~Gauges()

Gauges::~Gauges() {
}

// }}}
}
}
