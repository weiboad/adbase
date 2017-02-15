#if !defined ADBASE_LOGGING_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_ASYNCLOGGING_HPP_
#define ADBASE_UTILITY_ASYNCLOGGING_HPP_

#include <thread>
#include <condition_variable>
#include <vector>

namespace adbase {

/**
 * @addtogroup logging adbase::Logging
 */
/*@{*/

class AsyncLogging {
public:
	AsyncLogging(const std::string& basename,
				 size_t rollSize,
				 int flushInterval = 3);
	AsyncLogging(const AsyncLogging&) = delete;
	void operator=(const AsyncLogging&) = delete;

	~AsyncLogging() {
		if (_running) {
			stop();	
		}
	}

	void append(const char* logline, int len);

	void start();

	void stop() {
		_running = false;
		_cond.notify_one();
	}

	static void deleteThread(std::thread *t);

private:
	void threadFunc(void *data);

	typedef adbase::detail::FixedBuffer<adbase::detail::kLargeBuffer> Buffer;
	typedef std::unique_ptr<Buffer> BufferPtr;
	typedef std::vector<BufferPtr> BufferVector;
	const int _flushInterval;
	bool _running;
	std::string _basename;
	size_t _rollSize;		
	mutable std::mutex _mut;
	std::condition_variable _cond;
	BufferPtr _currentBuffer;	
	BufferPtr _nextBuffer;	
	BufferVector _buffers;

	typedef std::unique_ptr<std::thread, decltype(&AsyncLogging::deleteThread)> ThreadPtr;
	typedef std::vector<ThreadPtr> ThreadPool;
	ThreadPool _threads;
};

/*@}*/
	
}
#endif
