#if !defined ADBASE_LOGGING_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_LOGGING_LOGGING_HPP_
#define ADBASE_LOGGING_LOGGING_HPP_

#include <adbase/Utility.hpp>
#include <cassert>
#include <functional>

namespace adbase {

/**
 * @addtogroup logging adbase::Logging
 */
/*@{*/

class TimeZone;
class Logger {
public:
	enum LogLevel {
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS,	
	};		

	// {{{ 编译器计算源文件路劲
	
	class SourceFile {
	public:
		template<int N>
		inline SourceFile(const char (&arr)[N]) :
			_data(arr),
			_size(N - 1) {
			const char* slash = strrchr(_data, '/');		
			if (slash) {
				_data = slash + 1;	
				_size -= static_cast<int>(_data - arr);
			}
		}	

		explicit SourceFile(const char* filename) :
			_data(filename) {
			const char* slash = strrchr(filename, '/');	
			if (slash) {
				_data = slash + 1;	
			}
			_size = static_cast<int>(strlen(_data));
		}
		
		const char* _data;
		int _size;
	};

	// }}}
	
	Logger(SourceFile file, int line);
	Logger(SourceFile file, int line, LogLevel level);
	Logger(SourceFile file, int line, LogLevel level, const char* func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger();

	LogStream& stream() {
		return _impl._stream;	
	}

	static LogLevel logLevel();
	static void setLogLevel(LogLevel level);

	typedef std::function<void (const char* msg, size_t len)> OutputFunc;
	typedef std::function<void ()> FlushFunc;
	static void setOutput(const OutputFunc& func);
	static void setFlush(const FlushFunc& func);
	static void setTimeZone(const TimeZone& tz);
	
private:
	// {{{ stream impl
	class Impl {
	public:	
		typedef Logger::LogLevel LogLevel;
		Impl(LogLevel level, int oldErrno, const SourceFile& file, int line);
		void formatTime();
		void finish();

		Timestamp _time;
		LogStream _stream;
		LogLevel _level;
		int _line;
		SourceFile _basename;
	};
	// }}}
	Impl _impl;
};



extern Logger::LogLevel gLogLevel;

inline Logger::LogLevel Logger::logLevel() {
	return gLogLevel;	
}

#define LOG_TRACE if (adbase::Logger::logLevel() <= adbase::Logger::TRACE) \
	adbase::Logger(__FILE__, __LINE__, adbase::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (adbase::Logger::logLevel() <= adbase::Logger::DEBUG) \
	adbase::Logger(__FILE__, __LINE__, adbase::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (adbase::Logger::logLevel() <= adbase::Logger::INFO) \
	adbase::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN adbase::Logger(__FILE__, __LINE__, adbase::Logger::WARN).stream()
#define LOG_ERROR adbase::Logger(__FILE__, __LINE__, adbase::Logger::ERROR).stream()
#define LOG_FATAL adbase::Logger(__FILE__, __LINE__, adbase::Logger::FATAL).stream()
#define LOG_SYSERR adbase::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL adbase::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int saveErrno);

#define CHECK_NOTNULL(val) \
	::adbase::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

template<typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr) {
	if (ptr == nullptr) {
		Logger(file, line, Logger::FATAL).stream() << names;	
	}

	return ptr;
}
/*@}*/
}
#endif
