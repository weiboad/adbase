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
	ADSERVER_HTTP_ADD_API(http, Server, metrics)
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

    std::unordered_map<std::string, std::string> metricItems;
	for (auto &t : gauges) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;
		if (name.moduleName == "self") {
			continue;
		}

        metricTags["value"] = std::to_string(t.second);
        std::string item = "{";
        for (auto &tag : metricTags) {
		    item += "\"" + tag.first + "\":\"" + tag.second + "\",";
        }
        item = adbase::rightTrim(item, ",");
        item += "}";
        metricItems[getKey(name.moduleName, name.metricName)] += item + ",";
	}


	for (auto &t : counters) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;
		if (name.moduleName == "self") {
			continue;
		}

        metricTags["value"] = std::to_string(t.second);
        std::string item = "{";
        for (auto &tag : metricTags) {
		    item += "\"" + tag.first + "\":\"" + tag.second + "\",";
        }
        item = adbase::rightTrim(item, ",");
        item += "}";

        metricItems[getKey(name.moduleName, name.metricName)] += item + ",";
	}

	for (auto &t : meters) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;
		if (name.moduleName == "self") {
			continue;
		}

        metricTags["count"] = std::to_string(t.second.count);
        metricTags["mean"] = std::to_string(t.second.meanRate);
        metricTags["min1"] = std::to_string(t.second.min1Rate);
        metricTags["min5"] = std::to_string(t.second.min5Rate);
        metricTags["min15"] = std::to_string(t.second.min15Rate);
        std::string item = "{";
        for (auto &tag : metricTags) {
		    item += "\"" + tag.first + "\":\"" + tag.second + "\",";
        }
        item = adbase::rightTrim(item, ",");
        item += "}";

        metricItems[getKey(name.moduleName, name.metricName)] += item + ",";
	}
	for (auto &t : histograms) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;
		if (name.moduleName == "self") {
			continue;
		}

        metricTags["min"] = std::to_string(t.second.min);
        metricTags["max"] = std::to_string(t.second.max);
        metricTags["stddev"] = std::to_string(t.second.stddev);
        metricTags["median"] = std::to_string(t.second.median);
        metricTags["percent75"] = std::to_string(t.second.percent75);
        metricTags["percent95"] = std::to_string(t.second.percent95);
        metricTags["percent98"] = std::to_string(t.second.percent98);
        metricTags["percent99"] = std::to_string(t.second.percent99);
        metricTags["percent999"] = std::to_string(t.second.percent999);
        std::string item = "{";
        for (auto &tag : metricTags) {
		    item += "\"" + tag.first + "\":" + tag.second + ",";
        }
        item = adbase::rightTrim(item, ",");
        item += "}";

        metricItems[getKey(name.moduleName, name.metricName)] += item + ",";
	}
	for (auto &t : timers) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;

        metricTags["count"] = std::to_string(t.second.meter.count);
        metricTags["mean"] = std::to_string(t.second.meter.meanRate);
        metricTags["min1"] = std::to_string(t.second.meter.min1Rate);
        metricTags["min5"] = std::to_string(t.second.meter.min5Rate);
        metricTags["min15"] = std::to_string(t.second.meter.min15Rate);
        metricTags["min"] = std::to_string(t.second.histogram.min);
        metricTags["max"] = std::to_string(t.second.histogram.max);
        metricTags["stddev"] = std::to_string(t.second.histogram.stddev);
        metricTags["median"] = std::to_string(t.second.histogram.median);
        metricTags["percent75"] = std::to_string(t.second.histogram.percent75);
        metricTags["percent95"] = std::to_string(t.second.histogram.percent95);
        metricTags["percent98"] = std::to_string(t.second.histogram.percent98);
        metricTags["percent99"] = std::to_string(t.second.histogram.percent99);
        metricTags["percent999"] = std::to_string(t.second.histogram.percent999);
        std::string item = "{";
        for (auto &tag : metricTags) {
		    item += "\"" + tag.first + "\":\"" + tag.second + "\",";
        }
        item = adbase::rightTrim(item, ",");
        item += "}";

        metricItems[getKey(name.moduleName, name.metricName)] += item + ",";
	}

	std::unordered_map<std::string, std::string> modulesItems;
	modulesItems["system"] = system;
	for (auto &t : metricItems) {
        std::vector<std::string> result = adbase::explode(t.first, 26);
		if (modulesItems.find(result[0]) == modulesItems.end()) {
			modulesItems[result[0]] = "{";
		}

		modulesItems[result[0]] += "\"" + result[1] + "\":[" + adbase::rightTrim(t.second, ",") + "],";  
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
// {{{ void Server::metrics()

void Server::metrics(adbase::http::Request* request, adbase::http::Response* response, void*) {
	(void)request;
	std::string result;
    std::string tagValue = request->getQuery("tags"); 
	std::unordered_map<std::string, std::string> tags;
    if (!tagValue.empty()) {
        std::vector<std::string> tagStrs = adbase::explode(tagValue, '|', true);
        for (auto &t: tagStrs) {
            std::vector<std::string> tagKV = adbase::explode(t, ':', true); 
            if (tagKV.size() == 2) {
                tags[tagKV[0]] = tagKV[1];
            }
        }
    }

    int count = 0;
	std::unordered_map<std::string, std::string> procs = adbase::procStats();
    tags["name"] = adbase::trim(procs["name"], "()");


    tags["service"] = request->getServerAddress();
    tags["version"] = VERSION;

	for (auto &t : procs) {
        std::string key = adbase::replace(".", "_", t.first, count);
        tags["metric_type"] = "gauges";
        result += formatMetric(key, toUint64(t.second), tags);
	}

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

    tags["metric_type"] = "gauges";
	for (auto &t : gauges) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;
		if (name.moduleName == "self") {
			continue;
		}

        for (auto &t : tags) {
            metricTags[t.first] = t.second;
        }
        std::string key = name.moduleName + "_" + name.metricName;
        key = adbase::replace(".", "_", key, count);
        result += formatMetric(key, t.second, metricTags);
	}
    tags["metric_type"] = "counters";
	for (auto &t : counters) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;
		if (name.moduleName == "self") {
			continue;
		}
        std::string key = name.moduleName + "_" + name.metricName;
        key = adbase::replace(".", "_", key, count);
        for (auto &t : tags) {
            metricTags[t.first] = t.second;
        }
        result += formatMetric(key, t.second, metricTags);
	}

    tags["metric_type"] = "meters";
	for (auto &t : meters) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;
		if (name.moduleName == "self") {
			continue;
		}
        std::string key = name.moduleName + "_" + name.metricName;
        key = adbase::replace(".", "_", key, count);
        for (auto &t : tags) {
            metricTags[t.first] = t.second;
        }
        metricTags["meter_type"] = "count";
        result += formatMetric(key, t.second.count, metricTags);
        metricTags["meter_type"] = "mean";
        result += formatMetric(key, toUint64(t.second.meanRate), metricTags);
        metricTags["meter_type"] = "min1";
        result += formatMetric(key, toUint64(t.second.min1Rate), metricTags);
        metricTags["meter_type"] = "min5";
        result += formatMetric(key, toUint64(t.second.min5Rate), metricTags);
        metricTags["meter_type"] = "min15";
        result += formatMetric(key, toUint64(t.second.min15Rate), metricTags);
	}

    tags["metric_type"] = "histograms";
	for (auto &t : histograms) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;
		if (name.moduleName == "self") {
			continue;
		}

        std::string key = name.moduleName + "_" + name.metricName;
        key = adbase::replace(".", "_", key, count);
        for (auto &t : tags) {
            metricTags[t.first] = t.second;
        }
        metricTags["histograms_type"] = "min";
        result += formatMetric(key, toUint64(t.second.min), metricTags);
        metricTags["histograms_type"] = "max";
        result += formatMetric(key, toUint64(t.second.max), metricTags);
        metricTags["histograms_type"] = "mean";
        result += formatMetric(key, toUint64(t.second.mean), metricTags);
        metricTags["histograms_type"] = "stddev";
        result += formatMetric(key, toUint64(t.second.stddev), metricTags);
        metricTags["histograms_type"] = "median";
        result += formatMetric(key, toUint64(t.second.median), metricTags);
        metricTags["histograms_type"] = "percent75";
        result += formatMetric(key, toUint64(t.second.percent75), metricTags);
        metricTags["histograms_type"] = "percent95";
        result += formatMetric(key, toUint64(t.second.percent95), metricTags);
        metricTags["histograms_type"] = "percent98";
        result += formatMetric(key, toUint64(t.second.percent98), metricTags);
        metricTags["histograms_type"] = "percent99";
        result += formatMetric(key, toUint64(t.second.percent99), metricTags);
        metricTags["histograms_type"] = "percent999";
        result += formatMetric(key, toUint64(t.second.percent999), metricTags);
	}

    tags["metric_type"] = "timers";
	for (auto &t : timers) {
		adbase::metrics::MetricName name = adbase::metrics::Metrics::getMetricName(t.first);
        std::unordered_map<std::string, std::string> metricTags = name.tags;

        std::string key = name.moduleName + "_" + name.metricName;
        key = adbase::replace(".", "_", key, count);
        for (auto &t : tags) {
            metricTags[t.first] = t.second;
        }
        metricTags["timers_type"] = "count";
        result += formatMetric(key, t.second.meter.count, metricTags);
        metricTags["timers_type"] = "mean";
        result += formatMetric(key, toUint64(t.second.meter.meanRate), metricTags);
        metricTags["timers_type"] = "min1";
        result += formatMetric(key, toUint64(t.second.meter.min1Rate), metricTags);
        metricTags["timers_type"] = "min5";
        result += formatMetric(key, toUint64(t.second.meter.min5Rate), metricTags);
        metricTags["timers_type"] = "min15";
        result += formatMetric(key, toUint64(t.second.meter.min15Rate), metricTags);
        metricTags["timers_type"] = "min";
        result += formatMetric(key, toUint64(t.second.histogram.min), metricTags);
        metricTags["timers_type"] = "max";
        result += formatMetric(key, toUint64(t.second.histogram.max), metricTags);
        metricTags["timers_type"] = "mean";
        result += formatMetric(key, toUint64(t.second.histogram.mean), metricTags);
        metricTags["timers_type"] = "stddev";
        result += formatMetric(key, toUint64(t.second.histogram.stddev), metricTags);
        metricTags["timers_type"] = "median";
        result += formatMetric(key, toUint64(t.second.histogram.median), metricTags);
        metricTags["timers_type"] = "percent75";
        result += formatMetric(key, toUint64(t.second.histogram.percent75), metricTags);
        metricTags["timers_type"] = "percent95";
        result += formatMetric(key, toUint64(t.second.histogram.percent95), metricTags);
        metricTags["timers_type"] = "percent98";
        result += formatMetric(key, toUint64(t.second.histogram.percent98), metricTags);
        metricTags["timers_type"] = "percent99";
        result += formatMetric(key, toUint64(t.second.histogram.percent99), metricTags);
        metricTags["timers_type"] = "percent999";
        result += formatMetric(key, toUint64(t.second.histogram.percent999), metricTags);
	}

    responseHeader(response);
    response->setContent(result);
    response->sendReply();
}

// }}}
// {{{ const std::string Server::formatMetric()

const std::string Server::formatMetric(std::string key, uint64_t value, std::unordered_map<std::string, std::string> tags) {
    std::string result = "adbase_"; 

    int count = 0;
    std::string keyFormat = adbase::replace(".", "_", key, count);
    result += keyFormat;
    if (!tags.empty()) {
        result += "{";
        for (auto &t : tags) {
            result += t.first; 
            result += "=\""; 
            result += t.second; 
            result += "\","; 
        }
        result = adbase::rightTrim(result, ",");
        result += "} ";
    }

    result += "  ";
    result += std::to_string(value) + "\n";
    return result;
}

// }}}
// {{{ uint64_t Server::toUint64()

uint64_t Server::toUint64(std::string value) {
    errno = 0;
    uint64_t result = static_cast<uint64_t>(strtoull(value.c_str(), nullptr, 10));
    if (errno != 0) {
        return 0;
    }
    return result;
}

// }}}
// {{{ uint64_t Server::toUint64()

uint64_t Server::toUint64(double value) {
    return static_cast<uint64_t>(value * 10000);
}

// }}}
// {{{ const std::string Server::getKey()

const std::string Server::getKey(const std::string& moduleName, const std::string& metricName) {
    std::string result = moduleName;
    result.append(1, 26);
    result.append(metricName);
    return result;
}

// }}}
}
}
//@ENDIF
