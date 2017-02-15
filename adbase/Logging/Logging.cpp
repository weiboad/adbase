#include <adbase/Logging.hpp>
#include <adbase/Utility.hpp>

#include <errno.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <sstream>

namespace adbase {

thread_local char tErrnoBuf[512];	
thread_local char tTime[32];
thread_local time_t tLastSecond;

const char * strerror_tl(int saveErrno) {
	return strerror_r(saveErrno, tErrnoBuf, sizeof tErrnoBuf);	
}

Logger::LogLevel initLogLevel() {
	if (::getenv("ADINF_LOG_TRACE")) {
		return Logger::TRACE;	
	} else if (::getenv("ADINF_LOG_DEBUG")) {
		return Logger::DEBUG;	
	} else {
		return Logger::INFO;	
	}
}

Logger::LogLevel gLogLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
	"TRACE ",
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
	"FATAL ",	
};

class T {
public:
	T(const char* str, unsigned len) :
		_str(str),
		_len(len) {
		assert(strlen(str) == _len);	
	}

	const char* _str;
	const unsigned _len;
};

inline LogStream& operator<<(LogStream& s, T v) {
	s.append(v._str, v._len);	
	return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) {
	s.append(v._data, v._size);	
	return s;
}

void defaultOutput(const char* msg, int len) {
	size_t n = fwrite(msg, 1, len, stdout);	
	(void)n;
}

void defaultFlush() {
	fflush(stdout);	
}

Logger::OutputFunc gOutput = std::bind(&defaultOutput, std::placeholders::_1, std::placeholders::_2);
Logger::FlushFunc gFlush = std::bind(&defaultFlush);
TimeZone gLogTimeZone;

Logger::Impl::Impl(LogLevel level, int saveErrno, const SourceFile& file, int line) :
	_time(Timestamp::now()),
	_stream(),
	_level(level),
	_line(line),
	_basename(file) {
	formatTime();		
	std::ostringstream ss;
	ss << std::this_thread::get_id();
	_stream << ss.str() << " ";
	_stream << T(LogLevelName[level], 6);
	if (saveErrno != 0) {
		_stream << strerror_tl(saveErrno) << " (errno=" << saveErrno << ") ";
	}
}

void Logger::Impl::formatTime() {
	int64_t microSecondsSinceEpoch = _time.microSecondsSinceEpoch();
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
	int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
	if (seconds != tLastSecond) {
		tLastSecond = seconds;	
		struct tm tmTime;
		if (gLogTimeZone.valid()) {
			tmTime = gLogTimeZone.toLocalTime(seconds);	
		} else {
			::gmtime_r(&seconds, &tmTime);
		}

		int len = snprintf(tTime, sizeof(tTime), "%4d%02d%02d %02d:%02d:%02d",
						   tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday,
						   tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
		assert(len == 17); (void)len;
	}

	if (gLogTimeZone.valid()) {
		Fmt us(".%06d ", microseconds);
		assert(us.length() == 8);
		_stream << T(tTime, 17) << T(us.data(), 8);
	} else {
		Fmt us(".%06dZ ", microseconds);
		assert(us.length() == 9);
		_stream << T(tTime, 17) << T(us.data(), 9);
	}
}

void Logger::Impl::finish() {
	_stream << " - " << _basename << ':' << _line << '\n';
}

Logger::Logger(SourceFile file, int line) :
	_impl(INFO, 0, file, line) {		
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func) :
	_impl(level, 0, file, line) {
	_impl._stream << func << ' ';		
}

Logger::Logger(SourceFile file, int line, LogLevel level) :
	_impl(level, 0, file, line) {
}

Logger::Logger(SourceFile file, int line, bool toAbort) :
	_impl(toAbort ? FATAL : ERROR, errno, file, line) {
}

Logger::~Logger() {
	_impl.finish();
	const LogStream::Buffer& buf(stream().buffer());	
	gOutput(buf.data(), buf.length());
	if (_impl._level == FATAL) {
		gFlush();	
		abort();
	}
}

void Logger::setLogLevel(Logger::LogLevel level) {
	gLogLevel = level;	
}

void Logger::setOutput(const OutputFunc& out) {
	gOutput = out;	
}

void Logger::setFlush(const FlushFunc& flush) {
	gFlush = flush;	
}

void Logger::setTimeZone(const TimeZone& tz) {
	gLogTimeZone = tz;
}

}
