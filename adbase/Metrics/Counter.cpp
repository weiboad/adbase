#include <adbase/Metrics.hpp>

namespace adbase {
namespace metrics {
// {{{ Counter::Counter()

Counter::Counter(const std::string moduleName, const std::string metricName,
			   Metrics* metrics, adbase::Queue<DataItem>* queue) :
	_moduleName(moduleName),
	_metricName(metricName),
	_queue(queue) {
	(void)metrics;
}

// }}}
// {{{ const std::string& Counter::getModuleName()

const std::string& Counter::getModuleName() {
	return _moduleName;
}

// }}}
// {{{ const std::string& Counter::getMetricName()

const std::string& Counter::getMetricName() {
	return _metricName;
}

// }}}
// {{{ void Counter::add()

void Counter::add(int64_t val) {
	DataItem item;
	item.type = METRICS_COUNTER;
	item.val  = val;
	item.context = this;
	_queue->push(item);
}

// }}}
// {{{ void Counter::dec()

void Counter::dec(int64_t val) {
	DataItem item;
	item.type = METRICS_COUNTER;
	item.val  = -val;
	item.context = this;
	_queue->push(item);
}

// }}}
// {{{ Counter::~Counter()

Counter::~Counter() {
}

// }}}
}
}
