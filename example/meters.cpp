#include <signal.h>
#include <adbase/Net.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Metrics.hpp>

adbase::metrics::Metrics* metrics = nullptr;
adbase::metrics::Meters* meters = nullptr;
adbase::EventLoop* gloop = nullptr;

void test(void*) {
    if (metrics != nullptr) {
        std::unordered_map<std::string, adbase::metrics::MeterItem> values = metrics->getMeters();
        for (auto &t : values) {
            LOG_INFO << t.first << " : ";
            LOG_INFO << "\tcount     = " << t.second.count;
            LOG_INFO << "\tmean rate = " << t.second.meanRate;
            LOG_INFO << "\t1-minute rate = " << t.second.min1Rate;
            LOG_INFO << "\t5-minute rate = " << t.second.min5Rate;
            LOG_INFO << "\t15-minute rate = " << t.second.min15Rate;
        }
    }

    int num = rand() % 1000;
    for (int i = 0; i < num; i++) {
        if (meters != nullptr) {
            meters->mark();
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
    /// 忽略Broken Pipe信号
    signal(SIGPIPE, SIG_IGN);
    /// 处理kill信号
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
    meters = adbase::metrics::Metrics::buildMeters("test", "request");

    eventloop->start();
    return 0;
}
