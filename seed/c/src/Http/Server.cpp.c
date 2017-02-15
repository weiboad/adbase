//@IF @adserver
#include "Server.hpp"
#include "Version.hpp"
#include <adbase/Metrics.hpp>

namespace adserver {
namespace http {
// {{{ Server::Server()

Server::Server(AdServerContext* context) :
	HttpInterface(context) {
}

// }}}
// {{{ void Server::registerLocation()

void Server::registerLocation(adbase::http::Server* http) {
	ADSERVER_HTTP_ADD_API(http, Server, status)
}

// }}}
// {{{ void Server::status()

void Server::status(adbase::http::Request* request, adbase::http::Response* response, void*) {
	(void)request;
	std::string result;
	// 如下 json 手动拼接为了减少对 json 库的依赖，在实际项目中推荐用 rapidjson
	std::unordered_map<std::string, std::string> procs = adbase::procStats();
	procs["version"]    = VERSION;
	procs["git_sha1"]   = GIT_SHA1;
	procs["git_dirty"]  = GIT_DIRTY;
	procs["build_id"]   = BUILD_ID;
	procs["build_type"] = BUILD_TYPE;
	std::string system = "{";
	for (auto &t : procs) {
		system +="\"" + t.first + "\":\"" + t.second + "\",";
	}
	system = adbase::rightTrim(system, ",");

	// Metrics
	std::unordered_map<std::string, int64_t> gauges;
	std::unordered_map<std::string, int64_t> counters;
	std::unordered_map<std::string, adbase::metrics::MeterItem> meters;
	std::unordered_map<std::string, adbase::metrics::HistogramsItem> histograms;
	std::unordered_map<std::string, adbase::metrics::TimersItem> timers;
	if (_context->metrics != nullptr) {
		gauges = _context->metrics->getGauges();
		counters = _context->metrics->getCounter();
		histograms = _context->metrics->getHistograms();
		meters = _context->metrics->getMeters();
		timers = _context->metrics->getTimers();
	}

	std::unordered_map<std::string, std::string> modulesItems;
	modulesItems["system"] = system;
	for (auto &t : gauges) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
		if (name.moduleName == "self") {
			continue;
		}
		if (modulesItems.find(name.moduleName) == modulesItems.end()) {
			modulesItems[name.moduleName] = "{";
		}

		modulesItems[name.moduleName] += "\"" + name.metricName + "\":" + std::to_string(t.second) + ",";
	}
	for (auto &t : counters) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
		if (name.moduleName == "self") {
			continue;
		}
		if (modulesItems.find(name.moduleName) == modulesItems.end()) {
			modulesItems[name.moduleName] = "{";
		}

		modulesItems[name.moduleName] += "\"" + name.metricName + "\":" + std::to_string(t.second) + ",";
	}
	for (auto &t : meters) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
		if (name.moduleName == "self") {
			continue;
		}
		if (modulesItems.find(name.moduleName) == modulesItems.end()) {
			modulesItems[name.moduleName] = "{";
		}

		std::string meterItem = "{\"count\":" + std::to_string(t.second.count) +
								",\"meanRate\":" + std::to_string(t.second.meanRate) +
								",\"min1Rate\":" + std::to_string(t.second.min1Rate) +
								",\"min5Rate\":" + std::to_string(t.second.min5Rate) +
								",\"min15Rate\":" + std::to_string(t.second.min15Rate) + "}";
					
		modulesItems[name.moduleName] += "\"" + name.metricName + "\":" + meterItem + ",";
	}
	for (auto &t : histograms) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
		if (name.moduleName == "self") {
			continue;
		}
		if (modulesItems.find(name.moduleName) == modulesItems.end()) {
			modulesItems[name.moduleName] = "{";
		}

		std::string hisItem = "{\"min\":" + std::to_string(t.second.min) +
								",\"max\":" + std::to_string(t.second.max) +
								",\"mean\":" + std::to_string(t.second.mean) +
								",\"stddev\":" + std::to_string(t.second.stddev) +
								",\"median\":" + std::to_string(t.second.median) +
								",\"percent75\":" + std::to_string(t.second.percent75) +
								",\"percent95\":" + std::to_string(t.second.percent95) +
								",\"percent98\":" + std::to_string(t.second.percent98) +
								",\"percent99\":" + std::to_string(t.second.percent99) +
								",\"percent999\":" + std::to_string(t.second.percent999) + "}";
					
		modulesItems[name.moduleName] += "\"" + name.metricName + "\":" + hisItem + ",";
	}
	for (auto &t : timers) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
		if (modulesItems.find(name.moduleName) == modulesItems.end()) {
			modulesItems[name.moduleName] = "{";
		}

		std::string timerItem = "{\"count\":" + std::to_string(t.second.meter.count) +
								",\"meanRate\":" + std::to_string(t.second.meter.meanRate) +
								",\"min1Rate\":" + std::to_string(t.second.meter.min1Rate) +
								",\"min5Rate\":" + std::to_string(t.second.meter.min5Rate) +
								",\"min15Rate\":" + std::to_string(t.second.meter.min15Rate) +
								",\"min\":" + std::to_string(t.second.histogram.min) +
								",\"max\":" + std::to_string(t.second.histogram.max) +
								",\"mean\":" + std::to_string(t.second.histogram.mean) +
								",\"stddev\":" + std::to_string(t.second.histogram.stddev) +
								",\"median\":" + std::to_string(t.second.histogram.median) +
								",\"percent75\":" + std::to_string(t.second.histogram.percent75) +
								",\"percent95\":" + std::to_string(t.second.histogram.percent95) +
								",\"percent98\":" + std::to_string(t.second.histogram.percent98) +
								",\"percent99\":" + std::to_string(t.second.histogram.percent99) +
								",\"percent999\":" + std::to_string(t.second.histogram.percent999) + "}";
					
		modulesItems[name.moduleName] += "\"" + name.metricName + "\":" + timerItem + ",";
	}

	result = "{";
	for (auto &t : modulesItems) {
		result += "\"" + t.first + "\":" + adbase::rightTrim(t.second, ",") + "},";  
	}
	result = adbase::rightTrim(result, ",");
	result += "}";

	responseJson(response, result, 0, "");
}

// }}}
}
}
//@ENDIF
