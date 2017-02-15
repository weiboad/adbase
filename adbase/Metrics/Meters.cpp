#include <adbase/Metrics.hpp>

namespace adbase {
namespace metrics {
// {{{ Meters::Meters()

Meters::Meters(const std::string moduleName, const std::string metricName, Metrics* metrics) :
	_moduleName(moduleName),
	_metricName(metricName),
	_metrics(metrics) {
	_counter = Metrics::buildCounter("self", moduleName + "_" + metricName);
}

// }}}
// {{{ const std::string& Meters::getModuleName()

const std::string& Meters::getModuleName() {
	return _moduleName;
}

// }}}
// {{{ const std::string& Meters::getMetricName()

const std::string& Meters::getMetricName() {
	return _metricName;
}

// }}}
// {{{ void Meters::mark()

void Meters::mark() {
	if (_counter != nullptr) {
		_counter->add(1);
	}
}

// }}}
// {{{ int64_t Meters::getCounter()

int64_t Meters::getCounter() {
	if (_metrics != nullptr) {
		return _metrics->getCounter("self", _moduleName + "_" + _metricName);
	}
	return 0;
}

// }}}
// {{{ Meters::~Meters()

Meters::~Meters() {
}

// }}}
}
}
