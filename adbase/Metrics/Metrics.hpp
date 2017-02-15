#if !defined ADBASE_METRICS_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_METRICS_METRICS_HPP_
#define ADBASE_METRICS_METRICS_HPP_

#include <adbase/Net.hpp>
#include <adbase/Utility.hpp>

namespace adbase {

/**
 * @addtogroup metrics adbase::Metrics
 */
/*@{*/

namespace metrics {
// {{{ typedefs
typedef std::function<int64_t ()> GaugesDataCallback;
class Metrics;
class Gauges;
class Counter;
class Meters;
class Histograms;
class Timers;

typedef struct metricName {
	std::string moduleName;
	std::string metricName;
} MetricName;

typedef enum dataType {
	METRICS_COUNTER = 0,
	METRICS_HISTOGRAMS = 1,
	METRICS_STOP = 999,
} DataType;
typedef struct dataItem {
	int64_t val;
	DataType type;	
	void* context;
} DataItem;

typedef struct meterItem {
	int64_t count;
	uint32_t time;
	double meanRate;
	double min1Rate;
	double min5Rate;
	double min15Rate;
} MeterItem;

typedef struct histogramsItem {
	double min;
	double max;
	double mean;
	double stddev;
	double median;
	double percent75;
	double percent95;
	double percent98;
	double percent99;
	double percent999;
} HistogramsItem;

typedef struct timersItem {
	HistogramsItem histogram;
	MeterItem meter;
} TimersItem;

// }}}
class Metrics {
public:
	Metrics(adbase::Timer* timer);
	static Metrics* init(adbase::Timer* timer);
	static void stop();
	void start();
	void innerStop();
	void threadFunc(void *data);
	static void deleteThread(std::thread *t);

	// Gauges
	static Gauges* buildGauges(const std::string moduleName, const std::string metricName,
						uint32_t interval, const GaugesDataCallback& func);
	Gauges* createGauges(const std::string moduleName, const std::string metricName,
						uint32_t interval, const GaugesDataCallback& func);
	const std::unordered_map<std::string, int64_t> getGauges();
	void gauge(void* args);

	// Counter
	static Counter* buildCounter(const std::string moduleName, const std::string metricName);
	Counter* createCounter(const std::string moduleName, const std::string metricName);
	const std::unordered_map<std::string, int64_t> getCounter();
	int64_t getCounter(const std::string& moduleName, const std::string& metricName);
	void setCounter(const DataItem& item);

	// Meters
	static Meters* buildMeters(const std::string moduleName, const std::string metricName);
	Meters* createMeters(const std::string moduleName, const std::string metricName);
	void setMeters(const DataItem& item);
	void meter1sec(void* args);
	void meter1min(void* args);
	void meter5min(void* args);
	void meter15min(void* args);
	const std::unordered_map<std::string, MeterItem> getMeters();
	MeterItem getMeters(const std::string& moduleName, const std::string& metricName);

	// Histograms 
	static Histograms* buildHistograms(const std::string moduleName, const std::string metricName, uint32_t interval);
	Histograms* createHistograms(const std::string moduleName, const std::string metricName, uint32_t interval);
	const std::unordered_map<std::string, HistogramsItem> getHistograms();
	HistogramsItem getHistograms(const std::string& moduleName, const std::string& metricName);
	void setHistograms(const DataItem& item);
	void histograms(void* args);

	// Timers
	static Timers* buildTimers(const std::string moduleName, const std::string metricName, uint32_t interval);
	Timers* createTimers(const std::string moduleName, const std::string metricName, uint32_t interval);
	const std::unordered_map<std::string, TimersItem> getTimers();
	TimersItem getTimers(const std::string& moduleName, const std::string& metricName);

	static const MetricName getMetricName(const std::string& name);
	~Metrics();

private:
	mutable std::mutex _mut;
	adbase::Timer* _timer;
	typedef std::unique_ptr<std::thread, decltype(&Metrics::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool Threads;
	adbase::Queue<DataItem> _queue;
	bool _running;

	// Gauges
	std::unordered_map<std::string, int64_t> _gaugesValues;
	std::unordered_map<std::string, Gauges*> _gaugesObjs;
	// Counter
	std::unordered_map<std::string, int64_t> _counterValues;
	std::unordered_map<std::string, Counter*> _counterObjs;
	// Meters
	std::unordered_map<std::string, std::vector<uint64_t>> _metersVecValues;
	std::unordered_map<std::string, MeterItem> _metersValues;
	std::unordered_map<std::string, Meters*> _metersObjs;
	// Histograms
	std::unordered_map<std::string, std::vector<uint64_t>*> _histogramsVecValues;
	std::unordered_map<std::string, HistogramsItem> _histogramsValues;
	std::unordered_map<std::string, Histograms*> _histogramsObjs;
	// Timers
	std::unordered_map<std::string, Timers*> _timersObjs;

	const std::string getKey(const std::string& moduleName, const std::string& metricName);
};

}

/*@}*/

}

#endif
