#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_TIMEZONE_HPP_
#define ADBASE_UTILITY_TIMEZONE_HPP_

#include <time.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <assert.h>
#include <memory>

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/

/** 
 *  @brief 时区工具类
 */
class TimeZone {
public:
	explicit TimeZone(const char* zonefile);
	TimeZone(int eastOfUtc, const char* tzname);
	TimeZone() {
	}

	bool valid() const {
		return static_cast<bool>(_data);
	}

	struct tm toLocalTime(time_t secondsSinceEpoch) const;
	time_t fromLocalTime(const struct tm&) const;

	/// gmtime(3)
	static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);

	/// timegm(3)
	static time_t fromUtcTime(const struct tm&);
	/// year in [1900..2500], month in [1..12], day in [1..31]
	static time_t fromUtcTime(int year, int month, int day, int hour, int minute, int seconds);

	struct Data;

private:
	std::shared_ptr<Data> _data;
};

/*@}*/

}

#endif
