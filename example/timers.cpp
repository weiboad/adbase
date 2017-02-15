#include <signal.h>
#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Metrics.hpp>

adbase::metrics::Metrics* metrics = nullptr;
adbase::metrics::Timers* timers = nullptr;
adbase::EventLoop* gloop = nullptr;

void test1(void*) {
	while(true) {
		int num = rand() % 10000;
		for (int i = 0; i < num; i++) {
			if (timers != nullptr) {
				adbase::metrics::Timer timer;
				timer.start();
				std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 2));	
				timers->setTimer(timer.stop());
			}
		}
	}
}
void test(void*) {
	if (metrics != nullptr) {
		std::unordered_map<std::string, adbase::metrics::TimersItem> values = metrics->getTimers();
		for (auto &t : values) {
			adbase::metrics::MetricName name;
			name = adbase::metrics::Metrics::getMetricName(t.first);
			LOG_INFO << name.moduleName << "." << name.metricName << " : ";
			LOG_INFO << "\tcount     = " << t.second.meter.count;
			LOG_INFO << "\tmean rate = " << t.second.meter.meanRate;
			LOG_INFO << "\t1-minute rate = " << t.second.meter.min1Rate;
			LOG_INFO << "\t5-minute rate = " << t.second.meter.min5Rate;
			LOG_INFO << "\t15-minute rate = " << t.second.meter.min15Rate;
			LOG_INFO << "\tmin    = " << t.second.histogram.min;
			LOG_INFO << "\tmax    = " << t.second.histogram.max;
			LOG_INFO << "\tmean   = " << t.second.histogram.mean;
			LOG_INFO << "\tstddev = " << t.second.histogram.stddev;
			LOG_INFO << "\tmedian = " << t.second.histogram.median;
			LOG_INFO << "\t75%   <= " << t.second.histogram.percent75;
			LOG_INFO << "\t95%   <= " << t.second.histogram.percent95;
			LOG_INFO << "\t98%   <= " << t.second.histogram.percent98;
			LOG_INFO << "\t99%   <= " << t.second.histogram.percent99;
			LOG_INFO << "\t99.9%   <= " << t.second.histogram.percent999;
		}
	}
}

// {{{ static void killSignal()

static void killSignal(const int sig) {
	(void)sig;
	if (gloop != nullptr) {
		gloop->stop();
		delete gloop;
		gloop = nullptr;
	}

	adbase::metrics::Metrics::stop();
	exit(0);
}

// }}}
// {{{ static void reloadConf()

static void reloadConf(const int sig) {
	(void)sig;
}

// }}}
// {{{ static void registerSignal()

static void registerSignal() {
	/* 忽略Broken Pipe信号 */
	signal(SIGPIPE, SIG_IGN);
	/* 处理kill信号 */
	signal(SIGINT,  killSignal);
	signal(SIGKILL, killSignal);
	signal(SIGQUIT, killSignal);
	signal(SIGTERM, killSignal);
	signal(SIGHUP,  killSignal);
	signal(SIGSEGV, killSignal);
	signal(SIGUSR1, reloadConf);
}

// }}}

int main(void) {
	registerSignal();
	adbase::EventLoop* eventloop = new adbase::EventLoop();
	gloop = eventloop;
	adbase::Timer timer(eventloop->getBase());
	metrics = adbase::metrics::Metrics::init(&timer);

	uint32_t interval = 1000;
	timer.runEvery(interval, std::bind(test, std::placeholders::_1), nullptr);
	timers = adbase::metrics::Metrics::buildTimers("test", "request", interval);

	std::thread* t = new std::thread(std::bind(test1, std::placeholders::_1), nullptr);

	eventloop->start();
	t->join();
	return 0;
}
