#include <adbase/Utility.hpp> 
#include <adbase/Logging.hpp> 

int main(void) {
    adbase::TimeZone timeZone(8 * 3600, "CTS");

    adbase::Timestamp timeStamp = adbase::Timestamp::now();
    time_t seconds = timeStamp.secondsSinceEpoch();

    LOG_INFO << seconds;
    LOG_INFO << "UTC:" << timeStamp.toFormattedString();

    // convert to local time from utc
    struct tm localTime = timeZone.toLocalTime(seconds);
    // convert to time_t from struct tm
    time_t localSeconds = timeZone.fromUtcTime(localTime);
    LOG_INFO << "Local time:" << adbase::Timestamp::fromUnixTime(localSeconds).toFormattedString();

    // convert to struct tm from time_t
    struct tm convertTm = timeZone.toUtcTime(localSeconds);
    // convert to utc from local time
    time_t utcSeconds = timeZone.fromLocalTime(convertTm);
    LOG_INFO << "UTC time:" << adbase::Timestamp::fromUnixTime(utcSeconds).toFormattedString();

    return 0;
}
