#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_FILESYSTEM_HPP_
#define ADBASE_UTILITY_FILESYSTEM_HPP_

#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/

class ReadSmallFile {
public:
	ReadSmallFile(std::string filename);	
	~ReadSmallFile();

	// return errno
	template<typename S>
	int readToString(int maxSize, 
					 S* context,
					 int64_t* fileSize,
					 int64_t* modifyTime,
					 int64_t* createTime);

	int readToBuffer(int *size);

	const char* buffer() const {
		return _buffer;	
	}

	static const int kBufferSize = 64 * 1024;

private:
	int _fd;
	int _err;
	char _buffer[kBufferSize];
};

template<typename S>
int readFile(std::string filename,
			 int maxSize,
			 S* context,
			 int64_t* fileSize = nullptr,
			 int64_t* modifyTime = nullptr,
			 int64_t* createTime = nullptr) {
	ReadSmallFile file(filename);
	return file.readToString(maxSize, context, fileSize, modifyTime, createTime);
}

class AppendFile {
public:
	explicit AppendFile(std::string filename);

	~AppendFile();

	void append(const char* logline, const size_t len);

	void flush();

	size_t writtenBytes() const {
		return _writtenBytes;	
	}

private:
	size_t write(const char* logline, size_t len);

	FILE* _fp;
	char _buffer[64 * 1024];
	size_t _writtenBytes;
};

/// 递归遍历目录, 注意该函数禁止遍历根目录，并且目录中存在软链也忽略，只能遍历正常文件和目录
void recursiveDir(std::string pathName, bool recursive, std::vector<std::string> excludes, std::vector<std::string> &pathInfo);

/// 递归创建目录
bool mkdirRecursive(std::string pathName, int mode, bool recursive = true);

/*@}*/
}

#endif

