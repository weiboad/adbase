#include <adbase/Logging.hpp>
#include <adbase/Utility.hpp>

namespace adbase {
// {{{ AsyncLogging::AsyncLogging()

AsyncLogging::AsyncLogging(const std::string& basename,
						   size_t rollSize,
						   int flushInterval) :
	_flushInterval(flushInterval),
	_running(false),
	_basename(basename),
	_rollSize(rollSize) {
	BufferPtr buffer1(new Buffer);
	_currentBuffer = std::move(buffer1);
	BufferPtr buffer2(new Buffer);
	_nextBuffer = std::move(buffer2);
	_currentBuffer->bzero();
	_nextBuffer->bzero();
	_buffers.reserve(16);
}

// }}}
// {{{ void AsyncLogging::append()

void AsyncLogging::append(const char* logline, int len) {
	std::lock_guard<std::mutex> lk(_mut);
	if (_currentBuffer->avail() > len) {
		_currentBuffer->append(logline, len);	
	} else {
		_buffers.push_back(std::move(_currentBuffer));	
		if (_nextBuffer) {
			_currentBuffer = std::move(_nextBuffer);	
		} else {
			_currentBuffer.reset(new Buffer); // Rarely happens
		}
		_currentBuffer->append(logline, len);
		_cond.notify_one();
	}
}

// }}}
// {{{ void AsyncLogging::threadFunc()

void AsyncLogging::threadFunc(void *data) {
	(void)data;
	assert(_running == true);
	LogFile output(_basename, _rollSize, false);
	BufferPtr newBuffer1(new Buffer);	
	BufferPtr newBuffer2(new Buffer);	
	newBuffer1->bzero();
	newBuffer2->bzero();
	BufferVector buffersToWrite;
	buffersToWrite.reserve(16);
	while (_running) {
		assert(newBuffer1 && newBuffer1->length() == 0);	
		assert(newBuffer2 && newBuffer2->length() == 0);	
		assert(buffersToWrite.empty());
		{
			std::unique_lock <std::mutex> lck(_mut);
			if (_buffers.empty()) {
				_cond.wait_for(lck, std::chrono::seconds(_flushInterval));
			}
			
			_buffers.push_back(std::move(_currentBuffer));	
			_currentBuffer = std::move(newBuffer1);
			buffersToWrite.swap(_buffers);
			if (!_nextBuffer) {
				_nextBuffer = std::move(newBuffer2);	
			}
		}

		assert(!buffersToWrite.empty());
		if (buffersToWrite.size() > 25) {
			char buf[256];
			snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
					 Timestamp::now().toFormattedString().c_str(),
					 buffersToWrite.size() - 2);	
			fputs(buf, stderr);
			output.append(buf, static_cast<int>(strlen(buf)));
			buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
		}

		for (size_t i = 0; i < buffersToWrite.size(); ++i) {
			output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());		
		}

		if (buffersToWrite.size() > 2) {
			buffersToWrite.resize(2);
		}

		if (!newBuffer1) {
			assert(!buffersToWrite.empty());	
			newBuffer1 = std::move(buffersToWrite[0]);
			newBuffer1->reset();
		}

		if (!newBuffer2) {
			assert(!buffersToWrite.empty());	
			newBuffer2 = std::move(buffersToWrite[1]);
			newBuffer2->reset();
		}

		buffersToWrite.clear();
		output.flush();
	}

	output.flush();
}

// }}}
// {{{ void AsyncLogging::start()

void AsyncLogging::start() {
	_running = true;	
	ThreadPtr Thread(new std::thread(std::bind(&AsyncLogging::threadFunc, this, std::placeholders::_1), nullptr), &AsyncLogging::deleteThread);
	_threads.push_back(std::move(Thread));
}

// }}}
// {{{ void AsyncLogging::deleteThread()

void AsyncLogging::deleteThread(std::thread *t) {
	t->join();
	delete t;	
}

// }}}
}
