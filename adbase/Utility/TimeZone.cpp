#include <adbase/Utility.hpp>

namespace adbase {
namespace detail {
// {{{ struct Transition

struct Transition {
	time_t gmttime;
	time_t localtime;
	int localtimeIdx;

	Transition(time_t t, time_t l, int localIdx) :
		gmttime(t),
		localtime(l),
		localtimeIdx(localIdx) {
	}
};

// }}}
// {{{ struct Comp

struct Comp {
	bool compareGmt;

	Comp(bool gmt) : compareGmt(gmt) {
	}

	bool operator()(const Transition& lhs, const Transition& rhs) const {
		if (compareGmt) {
			return lhs.gmttime < rhs.gmttime;
		} else {
			return lhs.localtime < rhs.localtime;
		}
	}

	bool equal(const Transition& lhs, const Transition& rhs) const {
		if (compareGmt) {
			return lhs.gmttime == rhs.gmttime;
		} else {
			return lhs.localtime == rhs.localtime;
		}
	}
};

// }}}
// {{{ struct Localtime

struct Localtime {
	time_t gmtOffset;
	bool isDst;
	int arrbIdx;

	Localtime(time_t offset, bool dst, int arrb) :
		gmtOffset(offset),
		isDst(dst),
		arrbIdx(arrb) {
	}
};

// }}}
// {{{ fillHMS()

inline void fillHMS(unsigned seconds, struct tm* utc) {
	utc->tm_sec = seconds % 60;
	unsigned minutes = seconds / 60;
	utc->tm_min = minutes % 60;
	utc->tm_hour = minutes / 60;
}
// }}}
}
const int kSecondsPerDay = 24*60*60;
struct TimeZone::Data {
	std::vector<detail::Transition> transitions;
	std::vector<detail::Localtime> localtimes;
	std::vector<std::string> names;
	std::string abbreviation;
};
namespace detail {
// {{{ class File
class File {
public:
	File(const char* file) : 
		_fp(::fopen(file, "rb")) {
	}

	~File() {
		if (_fp) {
			::fclose(_fp);
		}
	}

	bool valid() const { return _fp; }

	std::string readBytes(int n) {
		char buf[n];
		ssize_t nr = ::fread(buf, 1, n, _fp);
		if (nr != n)
			throw std::logic_error("no enough data");
		return std::string(buf, n);
	}

	int32_t readInt32() {
		int32_t x = 0;
		ssize_t nr = ::fread(&x, 1, sizeof(int32_t), _fp);
		if (nr != sizeof(int32_t))
			throw std::logic_error("bad int32_t data");
		return networkToHost32(x);
	}

	uint8_t readUInt8() {
		uint8_t x = 0;
		ssize_t nr = ::fread(&x, 1, sizeof(uint8_t), _fp);
		if (nr != sizeof(uint8_t))
			throw std::logic_error("bad uint8_t data");
		return x;
	}

private:
	FILE* _fp;
};

// }}}
// {{{ bool readTimeZoneFile()

bool readTimeZoneFile(const char* zonefile, struct TimeZone::Data* data) {
	File f(zonefile);
	if (f.valid()) {
		try {
			std::string head = f.readBytes(4);
			if (head != "TZif")
				throw std::logic_error("bad head");
			std::string version = f.readBytes(1);
			f.readBytes(15);

			int32_t isgmtcnt = f.readInt32();
			int32_t isstdcnt = f.readInt32();
			int32_t leapcnt = f.readInt32();
			int32_t timecnt = f.readInt32();
			int32_t typecnt = f.readInt32();
			int32_t charcnt = f.readInt32();

			std::vector<int32_t> trans;
			std::vector<int> localtimes;
			trans.reserve(timecnt);
			for (int i = 0; i < timecnt; ++i) {
				trans.push_back(f.readInt32());
			}

			for (int i = 0; i < timecnt; ++i) {
				uint8_t local = f.readUInt8();
				localtimes.push_back(local);
			}

			for (int i = 0; i < typecnt; ++i) {
				int32_t gmtoff = f.readInt32();
				uint8_t isdst = f.readUInt8();
				uint8_t abbrind = f.readUInt8();

				data->localtimes.push_back(Localtime(gmtoff, isdst, abbrind));
			}

			for (int i = 0; i < timecnt; ++i) {
				int localIdx = localtimes[i];
				time_t localtime = trans[i] + data->localtimes[localIdx].gmtOffset;
				data->transitions.push_back(Transition(trans[i], localtime, localIdx));
			}

			data->abbreviation = f.readBytes(charcnt);

			// leapcnt
			for (int i = 0; i < leapcnt; ++i) {
				// int32_t leaptime = f.readInt32();
				// int32_t cumleap = f.readInt32();
			}
			// FIXME
			(void) isstdcnt;
			(void) isgmtcnt;
		} catch (std::logic_error& e) {
			fprintf(stderr, "%s\n", e.what());
		}
	}
	return true;
}

// }}}
// {{{ const Localtime* findLocaltime()

const Localtime* findLocaltime(const TimeZone::Data& data, Transition sentry, Comp comp) {
	const Localtime* local = NULL;

	if (data.transitions.empty() || comp(sentry, data.transitions.front())) {
		// FIXME: should be first non dst time zone
		local = &data.localtimes.front();
	} else {
		std::vector<Transition>::const_iterator transI = lower_bound(data.transitions.begin(),
																data.transitions.end(),
																sentry, comp);
		if (transI != data.transitions.end()) {
			if (!comp.equal(sentry, *transI)) {
				assert(transI != data.transitions.begin());
				--transI;
			}
			local = &data.localtimes[transI->localtimeIdx];
		} else {
			// FIXME: use TZ-env
			local = &data.localtimes[data.transitions.back().localtimeIdx];
		}
	}

	return local;
}

// }}}
}
// {{{ TimeZone::TimeZone()

TimeZone::TimeZone(const char* zonefile) : 
	_data(new TimeZone::Data) {
	if (!detail::readTimeZoneFile(zonefile, _data.get())) {
		_data.reset();
	}
}

// }}}
// {{{ TimeZone::TimeZone()

TimeZone::TimeZone(int eastOfUtc, const char* name) :
	_data(new TimeZone::Data) {
	_data->localtimes.push_back(detail::Localtime(eastOfUtc, false, 0));
	_data->abbreviation = name;
}

// }}}
// {{{ struct tm TimeZone::toLocalTime()

struct tm TimeZone::toLocalTime(time_t seconds) const {
	struct tm localTime;
	bzero(&localTime, sizeof(localTime));
	assert(_data != NULL);
	const Data& data(*_data);

	detail::Transition sentry(seconds, 0, 0);
	const detail::Localtime* local = findLocaltime(data, sentry, detail::Comp(true));

	if (local) {
		time_t localSeconds = seconds + local->gmtOffset;
		::gmtime_r(&localSeconds, &localTime); // FIXME: fromUtcTime
		localTime.tm_isdst = local->isDst;
		localTime.tm_gmtoff = local->gmtOffset;
		localTime.tm_zone = &data.abbreviation[local->arrbIdx];
	}

	return localTime;
}

// }}}
// {{{ time_t TimeZone::fromLocalTime()

time_t TimeZone::fromLocalTime(const struct tm& localTm) const {
	assert(_data != NULL);
	const Data& data(*_data);

	struct tm tmp = localTm;
	time_t seconds = ::timegm(&tmp); // FIXME: toUtcTime
	detail::Transition sentry(0, seconds, 0);
	const detail::Localtime* local = findLocaltime(data, sentry, detail::Comp(false));
	if (localTm.tm_isdst) {
		struct tm tryTm = toLocalTime(seconds - local->gmtOffset);
		if (!tryTm.tm_isdst
				&& tryTm.tm_hour == localTm.tm_hour
				&& tryTm.tm_min == localTm.tm_min) {
			// FIXME: HACK
			seconds -= 3600;
		}
	}
	return seconds - local->gmtOffset;
}

// }}}
// {{{ struct tm TimeZone::toUtcTime()

struct tm TimeZone::toUtcTime(time_t secondsSinceEpoch, bool yday) {
	struct tm utc;
	bzero(&utc, sizeof(utc));
	utc.tm_zone = "GMT";
	int seconds = static_cast<int>(secondsSinceEpoch % kSecondsPerDay);
	int days = static_cast<int>(secondsSinceEpoch / kSecondsPerDay);
	if (seconds < 0) {
		seconds += kSecondsPerDay;
		--days;
	}
	detail::fillHMS(seconds, &utc);
	Date date(days + Date::kJulianDayOf1970_01_01);
	Date::YearMonthDay ymd = date.yearMonthDay();
	utc.tm_year = ymd.year - 1900;
	utc.tm_mon = ymd.month - 1;
	utc.tm_mday = ymd.day;
	utc.tm_wday = date.weekDay();

	if (yday) {
		Date startOfYear(ymd.year, 1, 1);
		utc.tm_yday = date.julianDayNumber() - startOfYear.julianDayNumber();
	}
	return utc;
}

// }}}
// {{{ time_t TimeZone::fromUtcTime()

time_t TimeZone::fromUtcTime(const struct tm& utc) {
	return fromUtcTime(utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
			utc.tm_hour, utc.tm_min, utc.tm_sec);
}

// }}}
// {{{ time_t TimeZone::fromUtcTime()

time_t TimeZone::fromUtcTime(int year, int month, int day,
		int hour, int minute, int seconds) {
	Date date(year, month, day);
	int secondsInDay = hour * 3600 + minute * 60 + seconds;
	time_t days = date.julianDayNumber() - Date::kJulianDayOf1970_01_01;
	return days * kSecondsPerDay + secondsInDay;
}

// }}}
}

