#include <adbase/Utility.hpp>
#include <cstdio>
#include <sys/time.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS // use PRIu64
#endif
#include <inttypes.h>

namespace adbase {
static_assert(sizeof(Timestamp) == sizeof(int64_t), "Timestamp sizeof not is int64_t");
// {{{ std::string Timestamp::toString()

std::string Timestamp::toString() const {
	char buf[32] = {0};
	int64_t seconds = _microSecondsSinceEpoch / kMicroSecondsPerSecond;
	int64_t microseconds = _microSecondsSinceEpoch % kMicroSecondsPerSecond;	
	snprintf(buf, sizeof(buf) - 1, "%" PRIu64 ".%06" PRIu64 , seconds, microseconds);
	return buf;
}

// }}}
// {{{ std::string Timestamp::toFormattedString()

std::string Timestamp::toFormattedString(bool showMicroseconds) const {
	char buf[32] = {0};
	time_t seconds = static_cast<time_t>(_microSecondsSinceEpoch / kMicroSecondsPerSecond);
	
	struct tm tm_time;
	gmtime_r(&seconds, &tm_time);
	
	if (showMicroseconds) {
		int microseconds = static_cast<int>(_microSecondsSinceEpoch % kMicroSecondsPerSecond);
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
				 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
				 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
				 microseconds);	
	}

	return buf;
}

// }}}
// {{{ Timestamp Timestamp::now()

Timestamp Timestamp::now() {
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	int64_t seconds = tv.tv_sec;
	return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);	
}

// }}}
}
