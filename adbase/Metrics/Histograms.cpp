#include <adbase/Metrics.hpp>

namespace adbase {
namespace metrics {
// {{{ Histograms::Histograms()

Histograms::Histograms(const std::string moduleName, const std::string metricName,
					   uint32_t interval, Metrics* metrics, adbase::Queue<DataItem>* queue) :
	_moduleName(moduleName),
	_metricName(metricName),
	_interval(interval),
	_queue(queue) {
	(void)metrics;
}

// }}}
// {{{ const std::string& Histograms::getModuleName()

const std::string& Histograms::getModuleName() {
	return _moduleName;
}

// }}}
// {{{ const std::string& Histograms::getMetricName()

const std::string& Histograms::getMetricName() {
	return _metricName;
}

// }}}
// {{{ uint32_t Histograms::interval()

uint32_t Histograms::interval() {
	return _interval;
}

// }}}
// {{{ void Histograms::update()

void Histograms::update(double val) {
	DataItem item;
	item.type = METRICS_HISTOGRAMS;
	item.val  = static_cast<uint64_t>(val * 1000);
	item.context = this;
	_queue->push(item);
}

// }}}
// {{{ Histograms::~Histograms()

Histograms::~Histograms() {
}

// }}}
}
}
