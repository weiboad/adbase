#include <adbase/Metrics.hpp>
#include <adbase/Logging.hpp>

namespace adbase {
namespace metrics {
Metrics* gMetric = nullptr;
// {{{ Metrics::Metrics()

Metrics::Metrics(adbase::Timer* timer) :
	_timer(timer),
	_running(true) {
}

// }}}
// {{{ Metrics* Metrics::init()

Metrics* Metrics::init(adbase::Timer* timer) {
	if (gMetric == nullptr) {
		gMetric = new Metrics(timer);
		gMetric->start();
	}
	return gMetric;
}

// }}}
// {{{ void Metrics::stop()

void Metrics::stop() {
	if (gMetric != nullptr) {
		gMetric->innerStop();
		delete gMetric;
	}
}

// }}}
// {{{ void Metrics::innerStop()

void Metrics::innerStop() {
	_running = false;
	DataItem item;
	item.type = METRICS_STOP;
	_queue.push(item);
}

// }}}
// {{{ void Metrics::start()

void Metrics::start() {
	ThreadPtr Thread(new std::thread(std::bind(&Metrics::threadFunc, this, std::placeholders::_1), nullptr), &Metrics::deleteThread);
	Threads.push_back(std::move(Thread));
}

// }}}
// {{{ void Metrics::threadFunc()

void Metrics::threadFunc(void* data) {
	(void)data;
	bool isExists = true;
	while(_running) {
		std::vector<DataItem> items;
		if (isExists) {
			for (int i = 0; i < 10000; i++) {
				DataItem item;
				isExists = _queue.tryPop(item);
				if (!isExists) {
					break;
				}
				items.push_back(item);
			}
		} else {
			DataItem item;
			_queue.waitPop(item);
			items.push_back(item);
			isExists = true;
		}
		
		for (auto &t : items) {
			switch (t.type) {
				case METRICS_COUNTER:
					setCounter(t);
					break;
				case METRICS_HISTOGRAMS:
					setHistograms(t);
					break;
				default:
					;;
			}
		}
	}
}

// }}}

// {{{ Gauges* Metrics::buildGauges()

Gauges* Metrics::buildGauges(const std::string moduleName, const std::string metricName,
							 uint32_t interval, const GaugesDataCallback& func) {
	if (gMetric == nullptr) {
		return nullptr;	
	}

	return gMetric->createGauges(moduleName, metricName, interval, func);
}

// }}}
// {{{ Gauges* Metrics::buildGaugesWithTag()

Gauges* Metrics::buildGaugesWithTag(const std::string moduleName, const std::string metricName, 
        const std::unordered_map<std::string, std::string>& tags,
		uint32_t interval, const GaugesDataCallback& func) {
    return Metrics::buildGauges(moduleName, Metrics::combineKey(metricName, tags), interval, func);
}

// }}}
// {{{ Gauges* Metrics::createGauges()

Gauges* Metrics::createGauges(const std::string moduleName, const std::string metricName,
							  uint32_t interval, const GaugesDataCallback& func) {
	std::string key = getKey(moduleName, metricName);
	std::lock_guard<std::mutex> lk(_mut);
	if (_gaugesObjs.find(key) == _gaugesObjs.end()) {
		_gaugesObjs[key] = new Gauges(moduleName, metricName, interval, this, func);
		_timer->runEvery(interval, std::bind(&Metrics::gauge, this, std::placeholders::_1), _gaugesObjs[key]); 
	}
	return _gaugesObjs[key];

}

// }}}
// {{{  const std::unordered_map<std::string, int64_t> Metrics::getGauges()

const std::unordered_map<std::string, int64_t> Metrics::getGauges() {
	std::lock_guard<std::mutex> lk(_mut);
	return _gaugesValues;
}

// }}}
// {{{ void Metrics::gauge()

void Metrics::gauge(void* args) {
	Gauges* gauges = reinterpret_cast<Gauges*>(args);
	if (gauges != nullptr) {
		GaugesDataCallback callback = gauges->getCallback();
		int64_t val = callback();
		std::string key = getKey(gauges->getModuleName(), gauges->getMetricName());
		std::lock_guard<std::mutex> lk(_mut);
		_gaugesValues[key] = val;
	}
}

// }}}

// {{{ Counter* Metrics::buildCounter()

Counter* Metrics::buildCounter(const std::string moduleName, const std::string metricName) {
	if (gMetric == nullptr) {
		return nullptr;	
	}

	return gMetric->createCounter(moduleName, metricName);
}

// }}}
// {{{ Counter* Metrics::buildCounterWithTag()

Counter* Metrics::buildCounterWithTag(const std::string moduleName, const std::string metricName, const std::unordered_map<std::string, std::string>& tags) {
    return buildCounter(moduleName, combineKey(metricName, tags));
}

// }}}
// {{{ Counter* Metrics::createCounter()

Counter* Metrics::createCounter(const std::string moduleName, const std::string metricName) {
	std::string key = getKey(moduleName, metricName);
	std::lock_guard<std::mutex> lk(_mut);
	if (_counterObjs.find(key) == _counterObjs.end()) {
		_counterObjs[key]   = new Counter(moduleName, metricName, this, &_queue);
		_counterValues[key] = 0;
	}
	return _counterObjs[key];

}

// }}}
// {{{  const std::unordered_map<std::string, int64_t> Metrics::getCounter()

const std::unordered_map<std::string, int64_t> Metrics::getCounter() {
	std::lock_guard<std::mutex> lk(_mut);
	return _counterValues;
}

// }}}
// {{{ int64_t Metrics::getCounter()

int64_t Metrics::getCounter(const std::string& moduleName, const std::string& metricName) {
	std::lock_guard<std::mutex> lk(_mut);
	std::string key = getKey(moduleName, metricName);
	if (_counterValues.find(key) != _counterValues.end()) {
		return _counterValues[key];
	} else {
		return 0;
	}
}

// }}}
// {{{ void Metrics::setCounter()

void Metrics::setCounter(const DataItem& item) {
	Counter* counter = reinterpret_cast<Counter*>(item.context);
	std::string key = getKey(counter->getModuleName(), counter->getMetricName());
	std::lock_guard<std::mutex> lk(_mut);
	_counterValues[key] += item.val;
}

// }}}

// {{{ Meters* Metrics::buildMeters()

Meters* Metrics::buildMeters(const std::string moduleName, const std::string metricName) {
	if (gMetric == nullptr) {
		return nullptr;	
	}

	return gMetric->createMeters(moduleName, metricName);
}

// }}}
// {{{ Meters* Metrics::buildMetersWithTag()

Meters* Metrics::buildMetersWithTag(const std::string moduleName, const std::string metricName, const std::unordered_map<std::string, std::string>& tags) {
    return buildMeters(moduleName, combineKey(metricName, tags));
}

// }}}
// {{{ Meters* Metrics::createMeters()

Meters* Metrics::createMeters(const std::string moduleName, const std::string metricName) {
	std::string key = getKey(moduleName, metricName);
	{
		std::lock_guard<std::mutex> lk(_mut);
		if (_metersObjs.find(key) != _metersObjs.end()) {
			return _metersObjs[key];
		}
	}
	
	_metersObjs[key]   = new Meters(moduleName, metricName, this);

	std::lock_guard<std::mutex> lk(_mut);
	std::vector<uint64_t> values;
	MeterItem item;
	item.time  = 1;
	item.count = 0;
	item.meanRate = 0;
	item.min1Rate = 0;
	item.min5Rate = 0;
	item.min15Rate = 0;
	_metersVecValues[key] = values;
	_metersValues[key] = item;
	_timer->runEvery(1000, std::bind(&Metrics::meter1sec, this, std::placeholders::_1), _metersObjs[key]); 
	_timer->runEvery(60 * 1000, std::bind(&Metrics::meter1min, this, std::placeholders::_1), _metersObjs[key]); 
	_timer->runEvery(5 * 60 * 1000, std::bind(&Metrics::meter5min, this, std::placeholders::_1), _metersObjs[key]); 
	_timer->runEvery(15 * 60 *1000, std::bind(&Metrics::meter15min, this, std::placeholders::_1), _metersObjs[key]); 
	return _metersObjs[key];
}

// }}}
// {{{ void Metrics::meter1sec()

void Metrics::meter1sec(void* args) {
	Meters* meters = reinterpret_cast<Meters*>(args);
	if (meters == nullptr) {
		return;
	}
	int64_t val = meters->getCounter();
	std::lock_guard<std::mutex> lk(_mut);
	if (val < 0) {
		val = 0;
	}
	std::string key = getKey(meters->getModuleName(), meters->getMetricName());
	MeterItem item = _metersValues[key];
	item.time += 1;
	item.count = val;
	item.meanRate = (static_cast<double>(item.count) / item.time);
	item.meanRate = static_cast<double>(static_cast<uint64_t>(item.meanRate * 1000)) / 1000;
	_metersValues[key]    = item;
}

// }}}
// {{{ void Metrics::meter1min()

void Metrics::meter1min(void* args) {
	Meters* meters = reinterpret_cast<Meters*>(args);
	if (meters == nullptr) {
		return;
	}
	int64_t val = meters->getCounter();
	std::lock_guard<std::mutex> lk(_mut);
	if (val < 0) {
		val = 0;
	}
	std::string key = getKey(meters->getModuleName(), meters->getMetricName());
	MeterItem item = _metersValues[key];
	if (_metersVecValues.find(key) == _metersVecValues.end()) {
		return;
	}
	std::vector<uint64_t> historyValues = _metersVecValues[key];
	historyValues.insert(historyValues.begin(), val);
	if (historyValues.size() > 15) {
		historyValues.pop_back();
	}
	if (historyValues.size() >= 2) {
		item.min1Rate = static_cast<double>(historyValues[0] - historyValues[1]) / 60;
		item.min1Rate = static_cast<double>(static_cast<uint64_t>(item.min1Rate * 1000)) / 1000;
	}
	_metersVecValues[key] = historyValues;
	_metersValues[key]    = item;
}

// }}}
// {{{ void Metrics::meter5min()

void Metrics::meter5min(void* args) {
	Meters* meters = reinterpret_cast<Meters*>(args);
	if (meters == nullptr) {
		return;
	}
	std::lock_guard<std::mutex> lk(_mut);
	std::string key = getKey(meters->getModuleName(), meters->getMetricName());
	MeterItem item = _metersValues[key];
	if (_metersVecValues.find(key) == _metersVecValues.end()) {
		return;
	}
	std::vector<uint64_t> historyValues = _metersVecValues[key];

	if (historyValues.size() >= 5) {
		item.min5Rate = static_cast<double>(historyValues[0] - historyValues[4]) / 300;
		item.min5Rate = static_cast<double>(static_cast<uint64_t>(item.min5Rate * 1000)) / 1000;
	}
	_metersVecValues[key] = historyValues;
	_metersValues[key]    = item;
}

// }}}
// {{{ void Metrics::meter15min()

void Metrics::meter15min(void* args) {
	Meters* meters = reinterpret_cast<Meters*>(args);
	if (meters == nullptr) {
		return;
	}
	std::lock_guard<std::mutex> lk(_mut);
	std::string key = getKey(meters->getModuleName(), meters->getMetricName());
	MeterItem item = _metersValues[key];
	if (_metersVecValues.find(key) == _metersVecValues.end()) {
		return;
	}
	std::vector<uint64_t> historyValues = _metersVecValues[key];

	if (historyValues.size() >= 15) {
		item.min15Rate = static_cast<double>(historyValues[0] - historyValues[14]) / 900;
		item.min15Rate = static_cast<double>(static_cast<uint64_t>(item.min15Rate * 1000)) / 1000;
	}
	_metersVecValues[key] = historyValues;
	_metersValues[key]    = item;
}

// }}}
// {{{  const std::unordered_map<std::string, MeterItem> Metrics::getMeters()

const std::unordered_map<std::string, MeterItem> Metrics::getMeters() {
	std::lock_guard<std::mutex> lk(_mut);
	return _metersValues;
}

// }}}
// {{{ MeterItem Metrics::getMeters()

MeterItem Metrics::getMeters(const std::string& moduleName, const std::string& metricName) {
	std::lock_guard<std::mutex> lk(_mut);
	std::string key = getKey(moduleName, metricName);
	if (_metersValues.find(key) != _metersValues.end()) {
		return _metersValues[key];
	} else {
		MeterItem item;	
		return item;
	}
}

// }}}

// {{{ Histograms* Metrics::buildHistograms()

Histograms* Metrics::buildHistograms(const std::string moduleName, const std::string metricName, uint32_t interval) {
	if (gMetric == nullptr) {
		return nullptr;	
	}

	return gMetric->createHistograms(moduleName, metricName, interval);
}

// }}}
// {{{ Histograms* Metrics::buildHistogramsWithTag()

Histograms* Metrics::buildHistogramsWithTag(const std::string moduleName, const std::string metricName, uint32_t interval, const std::unordered_map<std::string, std::string>& tags) {
    return buildHistograms(moduleName, combineKey(metricName, tags), interval);
}

// }}}
// {{{ Histograms* Metrics::createHistograms()

Histograms* Metrics::createHistograms(const std::string moduleName, const std::string metricName, uint32_t interval) {
	std::string key = getKey(moduleName, metricName);
	std::lock_guard<std::mutex> lk(_mut);
	if (_histogramsObjs.find(key) == _histogramsObjs.end()) {
		std::vector<uint64_t>* values = new std::vector<uint64_t>;
		HistogramsItem item;
		item.min  = 0;
		item.max  = 0;
		item.mean = 0;
		item.stddev = 0;
		item.median = 0;
		item.percent75 = 0;
		item.percent95 = 0;
		item.percent98 = 0;
		item.percent99 = 0;
		item.percent999 = 0;
		_histogramsVecValues[key] = values;
		_histogramsValues[key] = item;
		_histogramsObjs[key] = new Histograms(moduleName, metricName, interval, this, &_queue);
		_timer->runEvery(interval, std::bind(&Metrics::histograms, this, std::placeholders::_1), _histogramsObjs[key]); 
	}
	return _histogramsObjs[key];
}

// }}}
// {{{  const std::unordered_map<std::string, HistogramsItem> Metrics::getHistograms()

const std::unordered_map<std::string, HistogramsItem> Metrics::getHistograms() {
	std::lock_guard<std::mutex> lk(_mut);
	return _histogramsValues;
}

// }}}
// {{{  HistogramsItem Metrics::getHistograms()

HistogramsItem Metrics::getHistograms(const std::string& moduleName, const std::string& metricName) {
	std::lock_guard<std::mutex> lk(_mut);
	std::string key = getKey(moduleName, metricName);
	if (_histogramsValues.find(key) != _histogramsValues.end()) {
		return _histogramsValues[key];
	} else {
		HistogramsItem item;	
		return item;
	}
}

// }}}
// {{{ void Metrics::setHistograms()

void Metrics::setHistograms(const DataItem& item) {
	Histograms* histograms = reinterpret_cast<Histograms*>(item.context);
	std::string key = getKey(histograms->getModuleName(), histograms->getMetricName());
	std::lock_guard<std::mutex> lk(_mut);
	std::vector<uint64_t>* values = _histogramsVecValues[key]; 
	uint64_t val = item.val < 0 ? 0 : item.val;
	values->push_back(val);
	_histogramsVecValues[key] = values;
}

// }}}
// {{{ void Metrics::histograms()

void Metrics::histograms(void* args) {
	Histograms* histograms = reinterpret_cast<Histograms*>(args);
	if (histograms != nullptr) {
		std::string key = getKey(histograms->getModuleName(), histograms->getMetricName());
		std::lock_guard<std::mutex> lk(_mut);
		std::vector<uint64_t>* values = _histogramsVecValues[key];
		if (values->empty()) {
			return;
		}
		HistogramsItem item;
		item.mean = static_cast<double>(std::accumulate(values->begin(), values->end(), 0)) / (static_cast<double>(values->size()) * 1000);
		item.mean = static_cast<double>(static_cast<uint64_t>(item.mean * 1000)) / 1000;
		if (values->size() >= 2) {
			double accum = 0.0;
			std::for_each(values->begin(), values->end(), [&](const double d) {  
				accum += (d - item.mean) * (d - item.mean);  
			}); 
			item.stddev = sqrt(accum / static_cast<double>(values->size() - 1)) / 1000; 
			item.stddev = static_cast<double>(static_cast<uint64_t>(item.stddev * 1000)) / 1000;
		}

		std::sort(values->begin(), values->end(), std::less<uint64_t>());
		item.min = static_cast<double>(values->at(0)) / 1000;
		item.max = static_cast<double>(values->at(values->size() - 1)) / 1000;
		size_t current = 0;
		int percentNum = 6;
		for (auto &t : *values) {
			current++;
			double percent = static_cast<double>(current) / static_cast<double>(values->size());
			if (percent >= 0.5 && percentNum == 6) {
				item.median = static_cast<double>(t) / 1000;
				percentNum--;
			}
			if (percent >= 0.75 && percentNum == 5) {
				item.percent75 = static_cast<double>(t) / 1000;
				percentNum--;
			}
			if (percent >= 0.95 && percentNum == 4) {
				item.percent95 = static_cast<double>(t) / 1000;
				percentNum--;
			}
			if (percent >= 0.98 && percentNum == 3) {
				item.percent98 = static_cast<double>(t) / 1000;
				percentNum--;
			}
			if (percent >= 0.99 && percentNum == 2) {
				item.percent99 = static_cast<double>(t) / 1000;
				percentNum--;
			}
			if (percent >= 0.999 && percentNum == 1) {
				item.percent999 = static_cast<double>(t) / 1000;
				percentNum--;
			}
			if (percentNum == 0) {
				break;
			}
		}
		values->clear();
		_histogramsVecValues[key] = values;
		_histogramsValues[key] = item;
	}
}

// }}}

// {{{ Timers* Metrics::buildTimers()

Timers* Metrics::buildTimers(const std::string moduleName, const std::string metricName, uint32_t interval) {
	if (gMetric == nullptr) {
		return nullptr;	
	}

	return gMetric->createTimers(moduleName, metricName, interval);
}

// }}}
// {{{ Timers* Metrics::buildTimersWithTag()

Timers* Metrics::buildTimersWithTag(const std::string moduleName, const std::string metricName, 
        const std::unordered_map<std::string, std::string>& tags,
        uint32_t interval) {
    return buildTimers(moduleName, combineKey(metricName, tags), interval);
}

// }}}
// {{{ Timers* Metrics::createTimers()

Timers* Metrics::createTimers(const std::string moduleName, const std::string metricName, uint32_t interval) {
	std::string key = getKey(moduleName, metricName);
	{
		std::lock_guard<std::mutex> lk(_mut);
		if (_timersObjs.find(key) != _timersObjs.end()) {
			return _timersObjs[key];
		}
	}
	
	_timersObjs[key]   = new Timers(moduleName, metricName, this, interval);
	return _timersObjs[key];
}

// }}}
// {{{  const std::unordered_map<std::string, TimersItem> Metrics::getTimers()

const std::unordered_map<std::string, TimersItem> Metrics::getTimers() {
	std::lock_guard<std::mutex> lk(_mut);
	std::unordered_map<std::string, TimersItem> timerItems;
	for (auto &t : _timersObjs) {
		Meters* meters = t.second->getMeters();
		std::string depKey = getKey(meters->getModuleName(), meters->getMetricName());
		HistogramsItem hisItem;
		memset(&hisItem, 0, sizeof(HistogramsItem));
		MeterItem metersItem;
		memset(&metersItem, 0, sizeof(MeterItem));
		if (_histogramsValues.find(depKey) != _histogramsValues.end()) {
			hisItem = _histogramsValues[depKey];
		}
		if (_metersValues.find(depKey) != _metersValues.end()) {
			metersItem = _metersValues[depKey];
		}
		TimersItem item;
		item.histogram = hisItem;
		item.meter = metersItem;
		timerItems[t.first] = item;
	}
	
	return timerItems;
}

// }}}
// {{{ TimersItem Metrics::getTimers()

TimersItem Metrics::getTimers(const std::string& moduleName, const std::string& metricName) {
	std::lock_guard<std::mutex> lk(_mut);
	std::string key = getKey(moduleName, metricName);
	Timers* timers = _timersObjs[key];
	Meters* meters = timers->getMeters();
	std::string depKey = getKey(meters->getModuleName(), meters->getMetricName());
	HistogramsItem hisItem;
	MeterItem metersItem;
	if (_histogramsValues.find(depKey) != _histogramsValues.end()) {
		hisItem = _histogramsValues[depKey];
	}
	if (_metersValues.find(depKey) != _metersValues.end()) {
		metersItem = _metersValues[depKey];
	}
	TimersItem item;
	item.histogram = hisItem;
	item.meter = metersItem;
	return item;
}

// }}}

// {{{ const std::string Metrics::getKey()

const std::string Metrics::getKey(const std::string& moduleName, const std::string& metricName) {
	std::string result = moduleName;
	result.append(1, 26);
	result.append(metricName);
	return result;
}

// }}}
// {{{ const std::string Metrics::combineKey()

const std::string Metrics::combineKey(const std::string& key, const std::unordered_map<std::string, std::string> tags) {
	std::string result = key;
	result.append(1, 26);
	result.append(Metrics::serializeTag(tags));
	return result;
}

// }}}
// {{{ const std::string Metrics::serializeTag()

const std::string Metrics::serializeTag(const std::unordered_map<std::string, std::string>& tags) {
	std::string result;
    for (auto &value : tags) {
        result.append(value.first);
        result.append(1, 31); // ASCII US (unit separator)
        result.append(value.second);
        result.append(1, 30); // ASCII RS (record separator)
    }

    std::string trimChar;
    trimChar.append(1, 30);
    return adbase::trim(result, trimChar.c_str());
}

// }}}
// {{{ const std::string Metrics::unserializeTag()

std::unordered_map<std::string, std::string> Metrics::unserializeTag(const std::string& tagStr) {
    std::unordered_map<std::string, std::string> result;
	std::vector<std::string> tagsStr = adbase::explode(tagStr, 30);
    for (auto &t : tagsStr) {
	    std::vector<std::string> tagInfo = adbase::explode(t, 31);
        if (tagInfo.size() == 2) {
            result[tagInfo[0]] = tagInfo[1];
        } 
    }
    return result;
}

// }}}
// {{{  const MetricName getMetricName(const std::string& name);

const MetricName Metrics::getMetricName(const std::string& name) {
	std::vector<std::string> result = adbase::explode(name, 26);
	MetricName metricName;
    if (result.size() < 2) {
        return metricName;
    }
    metricName.moduleName = result[0];
    metricName.metricName = result[1];
	if (result.size() == 3) {
        metricName.tags = unserializeTag(result[2]);
	}
	return metricName;
}

// }}}
// {{{ Metrics::~Metrics()

Metrics::~Metrics() {
}

// }}}
// {{{ void Metrics::deleteThread()

void Metrics::deleteThread(std::thread *t) {
    t->join();
    delete t;
}

// }}}
}
}
