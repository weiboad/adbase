#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <sys/stat.h>
#include <dirent.h>

namespace adbase {
// {{{ AppendFile::AppendFile() 

AppendFile::AppendFile(std::string filename) :
	_fp(::fopen(filename.c_str(), "ae")),
	_writtenBytes(0) {
	assert(_fp);
	::setbuffer(_fp, _buffer, sizeof _buffer);
}

// }}}
// {{{ AppendFile::~AppendFile()

AppendFile::~AppendFile() {
	::fclose(_fp);	
} 

// }}}
// {{{ void AppendFile::append()

void AppendFile::append(const char* logline, const size_t len) {
	size_t n = write(logline, len);	
	size_t remain = len - n;
	while (remain > 0) {
		size_t x = write(logline + n, remain);
		if (x == 0) {
			int err = ferror(_fp);	
			if (err) {
				// todo
				//fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));	
			}
			break;
		}	
		n += x;
		remain = len - n;
	}

	_writtenBytes += len;
}

// }}}
// {{{ void AppendFile::flush()

void AppendFile::flush() {
	::fflush(_fp);	
}

// }}}
// {{{ size_t AppendFile::write()

size_t AppendFile::write(const char* logline, size_t len) {
	return ::fwrite_unlocked(logline, 1, len, _fp);	
}

// }}}
// {{{ ReadSmallFile::ReadSmallFile()

ReadSmallFile::ReadSmallFile(std::string filename) :
	_fd(::open(filename.c_str(), O_RDONLY)),
	_err(0) {
	_buffer[0] = '\0';		
	if (_fd < 0) {
		_err = errno;	
	}

	int flags = fcntl(_fd, F_GETFD); 
	fcntl(_fd, F_SETFD, flags | FD_CLOEXEC);  
}

// }}}
// {{{ ReadSmallFile::~ReadSmallFile()

ReadSmallFile::~ReadSmallFile() {
	if (_fd >= 0) {
		::close(_fd);	
	}	
}

// }}}
// {{{ int ReadSmallFile::readToString();

// return errno
template<typename S>
int ReadSmallFile::readToString(int maxSize,
										  S* content,
										  int64_t* fileSize,
										  int64_t* modifyTime,
										  int64_t* createTime) {
	//static_assert(sizeof(off_t) == 8, "Type off_t sizeof is not 8");
	assert(content != nullptr);	
	int err = _err;
	if (_fd >= 0) {
		content->clear();		
		if (fileSize) {
			struct stat statbuf;
			if (::fstat(_fd, &statbuf) == 0) {
				if (S_ISREG(statbuf.st_mode)) {
					*fileSize = statbuf.st_size;	
					content->reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize), *fileSize)));
				} else if (S_ISDIR(statbuf.st_mode)) {
					err = EISDIR;	
				}

				if (modifyTime) {
					*modifyTime = statbuf.st_mtime;	
				}
				
				if (createTime) {
					*createTime = statbuf.st_ctime;	
				}
			} else {
				err = errno;	
			}
		}

		while (content->size() < static_cast<size_t>(maxSize)) {
			size_t toRead = std::min(static_cast<size_t>(maxSize) - content->size(), sizeof(_buffer));	
			ssize_t n = ::read(_fd, _buffer, toRead);
			if (n > 0) {
				content->append(_buffer, n);	
			} else {
				if (n < 0) {
					err = errno;	
				}
				break;
			}
		}
	}

	return err;
}

// }}}
// {{{ int ReadSmallFile::readToBuffer()

int ReadSmallFile::readToBuffer(int* size) {
	int err = _err;
	if (_fd >= 0) {
		ssize_t n = ::pread(_fd, _buffer, sizeof(_buffer) - 1, 0);	
		if (n >= 0) {
			if (size) {
				*size = static_cast<int>(n);	
			}	
			_buffer[n] = '\0';
		} else {
			err = errno;	
		}
	}	

	return err;
}

// }}}
// {{{ explicit instance

template int readFile(std::string filename,
					  int maxSize,
					  std::string* content,
					  int64_t*, int64_t*, int64_t*);
template int ReadSmallFile::readToString(int maxSize,
										 std::string* content,
										 int64_t*, int64_t*, int64_t*);
// }}}
// {{{ void recursiveDir()

void recursiveDir(std::string pathName, bool recursive, std::vector<std::string> excludes, std::vector<std::string> &pathInfo) {
    std::string::size_type s = pathName.find_last_not_of("/");
    if (s == std::string::npos) { // 不遍历根目录
        return;
    }
    pathName = pathName.substr(0, s + 1);
    for (auto &t : excludes) {
        std::string::size_type n = t.find_last_not_of("/");
        if (n == std::string::npos) { // 不遍历根目录
            continue;
        }
        t = t.substr(0, n + 1);
    }

    if (access(pathName.c_str(), R_OK) != 0) { // 没有读权限
        return;
    }

    // 过滤掉排除的路劲
    auto it = std::find(excludes.begin(), excludes.end(), pathName);
    if (it != excludes.end()) {
        return;
    }

    DIR *dir;
    struct dirent *ptr;
    dir = opendir(pathName.c_str());
    while ((ptr = readdir(dir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }

        struct stat fileStat;
        std::string newPathName = pathName;
        newPathName.append("/");
        newPathName.append(ptr->d_name);
        if (lstat(newPathName.c_str(), &fileStat) != 0) {
			closedir(dir);
            return;
        }

        if (S_ISDIR(fileStat.st_mode) && !S_ISLNK(fileStat.st_mode) && recursive) {
            std::vector<std::string> newPathInfo;
            recursiveDir(newPathName, true, excludes, newPathInfo);
            for (auto &t : newPathInfo) {
                pathInfo.push_back(t);
            }
        } else {
            pathInfo.push_back(newPathName);
        }
    }
    closedir(dir);
}

// }}}
// {{{ bool mkdirRecursive()

bool mkdirRecursive(std::string pathName, int mode, bool recursive) {
    if (access(pathName.c_str(), F_OK) != 0) { // 目录不存在
		std::string::size_type end   = pathName.find_last_of("/");
		std::string::size_type start = pathName.find_last_not_of("/");
		if (start == std::string::npos) {
			return true;
		}
		std::string prevDirPath = pathName.substr(0, end);
		if (recursive && prevDirPath != pathName) {
			if (!mkdirRecursive(prevDirPath, mode, true)) {
				return false;
			}
        }
		if (mkdir(pathName.c_str(), mode) == 0) {
			return true;
		}
    } else {
		return true;
	}
	return false;
}

// }}}
}
