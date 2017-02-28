#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <time.h>

int main(void) {
    // get now time
    adbase::Timestamp time = adbase::Timestamp::now();
    LOG_INFO << time.toFormattedString();

    // get microSecondsSinceEpoch
    LOG_INFO << time.microSecondsSinceEpoch();

    time_t times = time.secondsSinceEpoch();
    LOG_INFO << times;

    time_t timet;
    ::time(&timet);
    LOG_INFO << adbase::Timestamp::fromUnixTime(timet).toFormattedString();

    adbase::Timestamp time1(1488171946600590);

    LOG_INFO << "Time diff: " << adbase::timeDifference(time, time1);

    adbase::Timestamp time3 = adbase::addTime(time, 3600);
    LOG_INFO << "Add time 3600s: " << time3.toFormattedString();
    return 0;
}
