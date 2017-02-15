#include <adbase/Metrics.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace metrics {
// {{{ Timers::Timers()

Timers::Timers(const std::string moduleName, const std::string metricName, Metrics* metrics, uint32_t interval) :
	_moduleName(moduleName),
	_metricName(metricName),
	_metrics(metrics) {
	_meters = Metrics::buildMeters("self", moduleName + "_" + metricName);
	_histograms = Metrics::buildHistograms("self", moduleName + "_" + metricName, interval);
	_startTime  = std::chrono::steady_clock::now(); 
}

// }}}
// {{{ const std::string& Timers::getModuleName()

const std::string& Timers::getModuleName() {
	return _moduleName;
}

// }}}
// {{{ const std::string& Timers::getMetricName()

const std::string& Timers::getMetricName() {
	return _metricName;
}

// }}}
// {{{ void Timers::setTimer()

void Timers::setTimer(double time) {
	if (_meters != nullptr) {
		_meters->mark();
	}
	if (_histograms != nullptr) {
		_histograms->update(time);
	}
}

// }}}
// {{{ Histograms* Timers::getHistograms()

Histograms* Timers::getHistograms() {
	return _histograms;
}

// }}}
// {{{ Meters* Timers::getMeters()

Meters* Timers::getMeters() {
	return _meters;
}

// }}}
// {{{ Timers::~Timers()

Timers::~Timers() {
}

// }}}
}
}
