#if !defined ADBASE_CONFIG_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_CONFIG_INICONFIG_HPP_
#define ADBASE_CONFIG_INICONFIG_HPP_

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace adbase {

/**
 * @addtogroup config adbase::Config
 */
/*@{*/

class IniConfig {
public:
	/// 检查一个配置节是否存在
	bool hasSection(const std::string &sectionName) const;

	/// 检查一个配置项是否存在
	bool hasOption(const std::string &sectionName, const std::string &optionName) const;

	/// 获取一个配置项
	std::string getOption(const std::string &sectionName, const std::string &optionName) const;

	/// 获取一个配置项
	uint32_t getOptionUint32(const std::string &sectionName, const std::string &optionName);

	/// 获取一个配置项
	uint64_t getOptionUint64(const std::string &sectionName, const std::string &optionName);

	/// 获取一个配置项
	bool getOptionBool(const std::string &sectionName, const std::string &optionName);

	/// 获取所有的节名称
	std::vector<std::string> sections() const;

	/// 获取某个节的配置项
	std::vector<std::string> options(const std::string &sectionName) const;

	/// 设置某个节的配置项
	void setOption(const std::string &sectionName, const std::string &optionName, const std::string &optionValue);

	/// 删除某个节的配置
	void removeSection(const std::string &sectionName);

	/// 删除某个节的配置项
	void removeOption(const std::string &sectionName, const std::string &optionName);

	/// 添加配置节点
	void addSection(const std::string &sectionName);

	/// 添加配置项
	void addOption(const std::string &sectionName, const std::string &optionName, const std::string &optionValue);

	/// 析构函数
	virtual ~IniConfig() {}

private:
	std::map<std::string, std::map<std::string, std::string> > config;
	inline void validateSection(const std::string &sectionName) const;
	inline void validateOption(const std::string &sectionName, const std::string &optionName) const;
};

class InvalidOptionException : public std::exception {
public:
	InvalidOptionException(const char *msg) : message(msg) {}
	virtual ~InvalidOptionException() throw() {}
	virtual const char *what() const throw() { return message; }

private:
	const char *message;
};

class InvalidSectionException : public std::exception {
public:
	InvalidSectionException(const char *msg) : message(msg) {}
	virtual ~InvalidSectionException() throw() {}
	virtual const char *what() const throw() { return message; }

private:
	const char *message;
};

class DuplicateOptionException : public std::exception {
public:
	DuplicateOptionException(const char *msg) : message(msg) {}
	virtual ~DuplicateOptionException() throw() {}
	virtual const char *what() const throw() { return message; }

private:
	const char *message;
};

class DuplicateSectionException : public std::exception {
public:
	DuplicateSectionException(const char *msg) : message(msg) {}
	virtual ~DuplicateSectionException() throw() {}
	virtual const char *what() const throw() { return message; }

private:
	const char *message;
};

/*@}*/

}

#endif
