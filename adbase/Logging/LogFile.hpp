#if !defined ADBASE_LOGGING_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_LOGGING_LOGFILE_HPP_
#define ADBASE_LOGGING_LOGFILE_HPP_

#include <thread>
#include <mutex>
#include <cassert>

namespace adbase {

/**
 * @addtogroup logging adbase::Logging
 */
/*@{*/

class AppendFile;
class LogFile {
public:
	LogFile(const std::string& basename,
			size_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3,
			int checkEveryN = 1024);	
	~LogFile();

	void append(const char* logline, int len);
	void flush();
	bool rollFile();

private:
	void appendUnlocked(const char* logline, int len);
	static std::string getLogFileName(const std::string& basename, time_t* now);

	const std::string _basename;
	const size_t _rollSize;
	const int _flushInterval;
	const int _checkEveryN;
	bool _threadSafe;

	int _count;
	time_t _startOfPeriod;
	time_t _lastRoll;
	time_t _lastFlush;
	const static int _kRollPerSeconds = 60 * 60 * 24;

	std::shared_ptr<AppendFile> _file;
	mutable std::mutex _mtx;	
};

/*@}*/
}

#endif
