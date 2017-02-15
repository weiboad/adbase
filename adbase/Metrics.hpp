#ifndef ADBASE_METRICS_HPP_
#define ADBASE_METRICS_HPP_

/** 
 * @defgroup metrics adbase::Metrics
 *
 * @brief adbase 度量工具库 (使用方法见该模块的描述信息)
 *
 * @par 使用方法
 *
 * @note 请勿直接引用对应头文件，adbase::Metric 模块统一引用<adbase/Metrics.hpp> 文件
 *
 * @par Example for gauges
 * 
 * @code
 * adbase::metrics::Metrics::buildGauges("test", "rss", interval, [](){
 *     return 100;
 * });
 * @endcode
 *
 * @par Example for gauges all code
 *
 * @code
 * #include <signal.h>
 * #include <adbase/Net.hpp>
 * #include <adbase/Logging.hpp>
 * #include <adbase/Metrics.hpp>
 * 
 * adbase::metrics::Metrics* metrics = nullptr;
 * adbase::EventLoop* gloop = nullptr;
 * 
 * void test(void*) {
 *     if (metrics != nullptr) {
 *         std::unordered_map<std::string, int64_t> values = metrics->getGauges();
 *         for (auto &t : values) {
 *             LOG_INFO << t.first << " -> " << t.second;
 *         }
 *     }
 * }
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (gloop != nullptr) {
 *         gloop->stop();
 *         delete gloop;
 *         gloop = nullptr;
 *     }
 * 
 *     adbase::metrics::Metrics::stop();
 *     exit(0);
 * }
 * 
 * // }}}
 * // {{{ static void reloadConf()
 * 
 * static void reloadConf(const int sig) {
 *     (void)sig;
 * }
 * 
 * // }}}
 * // {{{ static void registerSignal()
 * 
 * static void registerSignal() {
 *     /// 忽略Broken Pipe信号 
 *     signal(SIGPIPE, SIG_IGN);
 *     /// 处理kill信号 
 *     signal(SIGINT,  killSignal);
 *     signal(SIGKILL, killSignal);
 *     signal(SIGQUIT, killSignal);
 *     signal(SIGTERM, killSignal);
 *     signal(SIGHUP,  killSignal);
 *     signal(SIGSEGV, killSignal);
 *     signal(SIGUSR1, reloadConf);
 * }
 * 
 * // }}}
 * 
 * int main(void) {
 *     registerSignal();
 *     adbase::EventLoop* eventloop = new adbase::EventLoop();
 *     gloop = eventloop;
 *     adbase::Timer timer(eventloop->getBase());
 *     metrics = adbase::metrics::Metrics::init(&timer);
 * 
 *     uint32_t interval = 1000;
 *     timer.runEvery(interval, std::bind(test, std::placeholders::_1), nullptr);
 *     adbase::metrics::Metrics::buildGauges("test", "rss", interval, [](){
 *         return 100;
 *     });
 *     eventloop->start();
 *     return 0;
 * }
 * @endcode
 *
 * @par Example for Counter
 * 
 * @code
 * adbase::metrics::Counter* counter = adbase::metrics::Metrics::buildCounter("test", "request");
 * @endcode
 *
 * @par Example for Counter all code
 *
 * @code
 * #include <signal.h>
 * #include <adbase/Net.hpp>
 * #include <adbase/Logging.hpp>
 * #include <adbase/Metrics.hpp>
 * 
 * adbase::metrics::Metrics* metrics = nullptr;
 * adbase::EventLoop* gloop = nullptr;
 * 
 * void test(void*) {
 *     if (metrics != nullptr) {
 *         std::unordered_map<std::string, int64_t> values = metrics->getCounter();
 *         for (auto &t : values) {
 *             LOG_INFO << t.first << " -> " << t.second;
 *         }
 *     }
 * }
 * 
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (gloop != nullptr) {
 *         gloop->stop();
 *         delete gloop;
 *         gloop = nullptr;
 *     }
 * 
 *     adbase::metrics::Metrics::stop();
 *     exit(0);
 * }
 * 
 * // }}}
 * // {{{ static void reloadConf()
 * 
 * static void reloadConf(const int sig) {
 *     (void)sig;
 * }
 * 
 * // }}}
 * // {{{ static void registerSignal()
 * 
 * static void registerSignal() {
 *     /// 忽略Broken Pipe信号
 *     signal(SIGPIPE, SIG_IGN);
 *     /// 处理kill信号
 *     signal(SIGINT,  killSignal);
 *     signal(SIGKILL, killSignal);
 *     signal(SIGQUIT, killSignal);
 *     signal(SIGTERM, killSignal);
 *     signal(SIGHUP,  killSignal);
 *     signal(SIGSEGV, killSignal);
 *     signal(SIGUSR1, reloadConf);
 * }
 * 
 * // }}}
 * 
 * int main(void) {
 *     registerSignal();
 *     adbase::EventLoop* eventloop = new adbase::EventLoop();
 *     gloop = eventloop;
 *     adbase::Timer timer(eventloop->getBase());
 *     //metrics = adbase::metrics::Metrics::init(&timer);
 * 
 *     uint32_t interval = 1000;
 *     timer.runEvery(interval, std::bind(test, std::placeholders::_1), nullptr);
 *     adbase::metrics::Counter* counter = adbase::metrics::Metrics::buildCounter("test", "request");
 *     if (counter != nullptr) {
 *         counter->add(9);
 *         counter->dec(3);
 *     }
 *     eventloop->start();
 *     return 0;
 * }
 * 
 * @endcode
 *
 * @par Example for Meters
 * 
 * @code
 * adbase::metrics::Meter* meters = adbase::metrics::Metrics::buildMeters("test", "request");
 * meters->mark();
 * @endcode
 *
 * @par Example for Meters all code
 *
 * @code
 * #include <signal.h>
 * #include <adbase/Net.hpp>
 * #include <adbase/Logging.hpp>
 * #include <adbase/Metrics.hpp>
 * 
 * adbase::metrics::Metrics* metrics = nullptr;
 * adbase::metrics::Meters* meters = nullptr;
 * adbase::EventLoop* gloop = nullptr;
 * 
 * void test(void*) {
 *     if (metrics != nullptr) {
 *         std::unordered_map<std::string, adbase::metrics::MeterItem> values = metrics->getMeters();
 *         for (auto &t : values) {
 *             LOG_INFO << t.first << " : ";
 *             LOG_INFO << "\tcount     = " << t.second.count;
 *             LOG_INFO << "\tmean rate = " << t.second.meanRate;
 *             LOG_INFO << "\t1-minute rate = " << t.second.min1Rate;
 *             LOG_INFO << "\t5-minute rate = " << t.second.min5Rate;
 *             LOG_INFO << "\t15-minute rate = " << t.second.min15Rate;
 *         }
 *     }
 * 
 *     int num = rand() % 1000;
 *     for (int i = 0; i < num; i++) {
 *         if (meters != nullptr) {
 *             meters->mark();
 *         }
 *     }
 * }
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (gloop != nullptr) {
 *         gloop->stop();
 *         delete gloop;
 *         gloop = nullptr;
 *     }
 * 
 *     adbase::metrics::Metrics::stop();
 *     exit(0);
 * }
 * 
 * // }}}
 * // {{{ static void reloadConf()
 * 
 * static void reloadConf(const int sig) {
 *     (void)sig;
 * }
 * 
 * // }}}
 * // {{{ static void registerSignal()
 * 
 * static void registerSignal() {
 *     /// 忽略Broken Pipe信号
 *     signal(SIGPIPE, SIG_IGN);
 *     /// 处理kill信号
 *     signal(SIGINT,  killSignal);
 *     signal(SIGKILL, killSignal);
 *     signal(SIGQUIT, killSignal);
 *     signal(SIGTERM, killSignal);
 *     signal(SIGHUP,  killSignal);
 *     signal(SIGSEGV, killSignal);
 *     signal(SIGUSR1, reloadConf);
 * }
 * 
 * // }}}
 * 
 * int main(void) {
 *     registerSignal();
 *     adbase::EventLoop* eventloop = new adbase::EventLoop();
 *     gloop = eventloop;
 *     adbase::Timer timer(eventloop->getBase());
 *     metrics = adbase::metrics::Metrics::init(&timer);
 * 
 *     uint32_t interval = 1000;
 *     timer.runEvery(interval, std::bind(test, std::placeholders::_1), nullptr);
 *     meters = adbase::metrics::Metrics::buildMeters("test", "request");
 * 
 *     eventloop->start();
 *     return 0;
 * }
 * @endcode
 *
 * @par Example for Histograms
 * 
 * @code
 * adbase::metrics::Histograms* histograms = adbase::metrics::Metrics::buildHistograms("test", "request", interval);
 * histograms->update(val);
 * @endcode
 *
 * @par Example for Histograms all code
 *
 * @code
 * #include <signal.h>
 * #include <adbase/Net.hpp>
 * #include <adbase/Logging.hpp>
 * #include <adbase/Metrics.hpp>
 * 
 * adbase::metrics::Metrics* metrics = nullptr;
 * adbase::metrics::Histograms* histograms = nullptr;
 * adbase::EventLoop* gloop = nullptr;
 * 
 * void test(void*) {
 *     if (metrics != nullptr) {
 *         std::unordered_map<std::string, adbase::metrics::HistogramsItem> values = metrics->getHistograms();
 *         for (auto &t : values) {
 *             LOG_INFO << t.first << " : ";
 *             LOG_INFO << "\tmin    = " << t.second.min;
 *             LOG_INFO << "\tmax    = " << t.second.max;
 *             LOG_INFO << "\tmean   = " << t.second.mean;
 *             LOG_INFO << "\tstddev = " << t.second.stddev;
 *             LOG_INFO << "\tmedian = " << t.second.median;
 *             LOG_INFO << "\t75%   <= " << t.second.percent75;
 *             LOG_INFO << "\t95%   <= " << t.second.percent95;
 *             LOG_INFO << "\t98%   <= " << t.second.percent98;
 *             LOG_INFO << "\t99%   <= " << t.second.percent99;
 *             LOG_INFO << "\t99.9%   <= " << t.second.percent999;
 *         }
 *     }
 * 
 *     for (int i = 0; i < 10000; i++) {
 *         if (histograms != nullptr) {
 *             histograms->update(rand() % 1000);
 *         }
 *     }
 * }
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (gloop != nullptr) {
 *         gloop->stop();
 *         delete gloop;
 *         gloop = nullptr;
 *     }
 * 
 *     adbase::metrics::Metrics::stop();
 *     exit(0);
 * }
 * 
 * // }}}
 * // {{{ static void reloadConf()
 * 
 * static void reloadConf(const int sig) {
 *     (void)sig;
 * }
 * 
 * // }}}
 * // {{{ static void registerSignal()
 * 
 * static void registerSignal() {
 *     /// 忽略Broken Pipe信号 
 *     signal(SIGPIPE, SIG_IGN);
 *     /// 处理kill信号 
 *     signal(SIGINT,  killSignal);
 *     signal(SIGKILL, killSignal);
 *     signal(SIGQUIT, killSignal);
 *     signal(SIGTERM, killSignal);
 *     signal(SIGHUP,  killSignal);
 *     signal(SIGSEGV, killSignal);
 *     signal(SIGUSR1, reloadConf);
 * }
 * 
 * // }}}
 * 
 * int main(void) {
 *     registerSignal();
 *     adbase::EventLoop* eventloop = new adbase::EventLoop();
 *     gloop = eventloop;
 *     adbase::Timer timer(eventloop->getBase());
 *     metrics = adbase::metrics::Metrics::init(&timer);
 * 
 *     uint32_t interval = 1000;
 *     timer.runEvery(interval, std::bind(test, std::placeholders::_1), nullptr);
 *     histograms = adbase::metrics::Metrics::buildHistograms("test", "request", interval);
 * 
 *     eventloop->start();
 *     return 0;
 * }
 * @endcode
 * 
 * @par Example for Timers
 * 
 * @code
 * adbase::metrics::Timers* timers = adbase::metrics::Metrics::buildTimers("test", "request", interval);
 * adbase::metrics::Timer timer.start();
 * // do something
 * timers->setTimer(timer.stop());
 * @endcode
 *
 * @par Example for Timers all code
 *
 * @code
 * #include <signal.h>
 * #include <adbase/Net.hpp>
 * #include <adbase/Logging.hpp>
 * #include <adbase/Metrics.hpp>
 * 
 * adbase::metrics::Metrics* metrics = nullptr;
 * adbase::metrics::Timers* timers = nullptr;
 * adbase::EventLoop* gloop = nullptr;
 * 
 * void test1(void*) {
 *     while(true) {
 *         int num = rand() % 10000;
 *         for (int i = 0; i < num; i++) {
 *             if (timers != nullptr) {
 *                 adbase::metrics::Timer timer;
 *                 timer.start();
 *                 std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 2));
 *                 timers->setTimer(timer.stop());
 *             }
 *         }
 *     }
 * }
 * void test(void*) {
 *     if (metrics != nullptr) {
 *         std::unordered_map<std::string, adbase::metrics::TimersItem> values = metrics->getTimers();
 *         for (auto &t : values) {
 *             adbase::metrics::MetricName name;
 *             name = adbase::metrics::Metrics::getMetricName(t.first);
 *             LOG_INFO << name.moduleName << "." << name.metricName << " : ";
 *             LOG_INFO << "\tcount     = " << t.second.meter.count;
 *             LOG_INFO << "\tmean rate = " << t.second.meter.meanRate;
 *             LOG_INFO << "\t1-minute rate = " << t.second.meter.min1Rate;
 *             LOG_INFO << "\t5-minute rate = " << t.second.meter.min5Rate;
 *             LOG_INFO << "\t15-minute rate = " << t.second.meter.min15Rate;
 *             LOG_INFO << "\tmin    = " << t.second.histogram.min;
 *             LOG_INFO << "\tmax    = " << t.second.histogram.max;
 *             LOG_INFO << "\tmean   = " << t.second.histogram.mean;
 *             LOG_INFO << "\tstddev = " << t.second.histogram.stddev;
 *             LOG_INFO << "\tmedian = " << t.second.histogram.median;
 *             LOG_INFO << "\t75%   <= " << t.second.histogram.percent75;
 *             LOG_INFO << "\t95%   <= " << t.second.histogram.percent95;
 *             LOG_INFO << "\t98%   <= " << t.second.histogram.percent98;
 *             LOG_INFO << "\t99%   <= " << t.second.histogram.percent99;
 *             LOG_INFO << "\t99.9%   <= " << t.second.histogram.percent999;
 *         }
 *     }
 * }
 * 
 * // {{{ static void killSignal()
 * 
 * static void killSignal(const int sig) {
 *     (void)sig;
 *     if (gloop != nullptr) {
 *         gloop->stop();
 *         delete gloop;
 *         gloop = nullptr;
 *     }
 * 
 *     adbase::metrics::Metrics::stop();
 *     exit(0);
 * }
 * 
 * // }}}
 * // {{{ static void reloadConf()
 * 
 * static void reloadConf(const int sig) {
 *     (void)sig;
 * }
 * 
 * // }}}
 * // {{{ static void registerSignal()
 * 
 * static void registerSignal() {
 *     /// 忽略Broken Pipe信号 
 *     signal(SIGPIPE, SIG_IGN);
 *     /// 处理kill信号 
 *     signal(SIGINT,  killSignal);
 *     signal(SIGKILL, killSignal);
 *     signal(SIGQUIT, killSignal);
 *     signal(SIGTERM, killSignal);
 *     signal(SIGHUP,  killSignal);
 *     signal(SIGSEGV, killSignal);
 *     signal(SIGUSR1, reloadConf);
 * }
 * 
 * // }}}
 * 
 * int main(void) {
 *     registerSignal();
 *     adbase::EventLoop* eventloop = new adbase::EventLoop();
 *     gloop = eventloop;
 *     adbase::Timer timer(eventloop->getBase());
 *     metrics = adbase::metrics::Metrics::init(&timer);
 * 
 *     uint32_t interval = 1000;
 *     timer.runEvery(interval, std::bind(test, std::placeholders::_1), nullptr);
 *     timers = adbase::metrics::Metrics::buildTimers("test", "request", interval);
 * 
 *     std::thread* t = new std::thread(std::bind(test1, std::placeholders::_1), nullptr);
 * 
 *     eventloop->start();
 *     t->join();
 *     return 0;
 * }
 * @endcode
 *
 */
/*@{*/ 

#include <adbase/Metrics/Metrics.hpp>
#include <adbase/Metrics/Gauges.hpp>
#include <adbase/Metrics/Counter.hpp>
#include <adbase/Metrics/Meters.hpp>
#include <adbase/Metrics/Histograms.hpp>
#include <adbase/Metrics/Timers.hpp>
#include <adbase/Metrics/Timer.hpp>

/*@}*/ 

#endif
