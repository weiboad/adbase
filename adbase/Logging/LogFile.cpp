#include <adbase/Logging.hpp>
#include <adbase/Utility.hpp>

namespace adbase {
// {{{ LogFile::LogFile()

LogFile::LogFile(const std::string& basename,
				 size_t rollSize,
				 bool threadSafe,
				 int flushInterval,
				 int checkEveryN) :
	_basename(basename),
	_rollSize(rollSize),
	_flushInterval(flushInterval),
	_checkEveryN(checkEveryN),
	_threadSafe(threadSafe),
	_count(0),
	_startOfPeriod(0),
	_lastRoll(0),
	_lastFlush(0) {
	if (basename.find_last_of("/") != std::string::npos) {
		std::string dirname =  basename.substr(0, basename.find_last_of("/"));
		struct stat fileStat;
		if (stat(dirname.c_str(), &fileStat) != 0 || !S_ISDIR(fileStat.st_mode)) {
			char buf[256];
			snprintf(buf, sizeof buf, "Log basename dir `%s` is not exists.\n", dirname.c_str());
			fputs(buf, stderr);
		}
	}
	rollFile();		
}

// }}}
// {{{ LogFile::~LogFile()

LogFile::~LogFile() {
}

// }}}
// {{{ void LogFile::append()

void LogFile::append(const char* logline, int len) {
	if (_threadSafe) {
		std::lock_guard<std::mutex> lk(_mtx);		
	}
	appendUnlocked(logline, len);
}

// }}}
// {{{ void LogFile::flush()

void LogFile::flush() {
	if (_threadSafe) {
		std::lock_guard<std::mutex> lk(_mtx);		
	}
	_file->flush();
}

// }}}
// {{{ void LogFile::appendUnlocked()

void LogFile::appendUnlocked(const char* logline, int len) {
	_file->append(logline, len);	

	if (_file->writtenBytes() > _rollSize) {
		rollFile();
	} else {
		++_count;	
		if (_count >= _checkEveryN) {
			_count = 0;	
			time_t now = ::time(nullptr);
			time_t thisPeriod = now / _kRollPerSeconds * _kRollPerSeconds;
			if (thisPeriod != _startOfPeriod) {
				rollFile();	
			} else if (now - _lastFlush > _flushInterval) {
				_lastFlush = now;
				_file->flush();	
			}
		}
	}
}

// }}}
// {{{ bool LogFile::rollFile()

bool LogFile::rollFile() {
	time_t now = 0;
	std::string filename = getLogFileName(_basename, &now);
	time_t start = now / _kRollPerSeconds * _kRollPerSeconds;	

	if (now > _lastRoll) {
		_lastRoll = now;
		_lastFlush = now;
		_startOfPeriod = start;
		_file.reset(new AppendFile(filename));
		return true;
	}
	return false;
}

// }}}
// {{{ std::string LogFile::getLogFileName()

std::string LogFile::getLogFileName(const std::string& basename, time_t* now) {
	std::string filename;
	filename.reserve(basename.size() + 64);	
	filename = basename;

	char timebuf[32];
	struct tm tm;
	*now = time(nullptr);
	gmtime_r(now, &tm);
	strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
	filename += timebuf;
	filename += hostname();

	char pidbuf[32];
	snprintf(pidbuf, sizeof pidbuf, ".%d", pid());
	filename += pidbuf;

	filename += ".log";

	return filename;
}

// }}}
}
