#if !defined ADBASE_CONFIG_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_CONFIG_INIPARSE_HPP_
#define ADBASE_CONFIG_INIPARSE_HPP_

#include <string>

namespace adbase {

/**
 * @addtogroup config adbase::Config
 */
/*@{*/

class IniConfig;
class IniParse {
public:
	/// 读取并解析配置字符串
	static IniConfig load(const std::string &iniStr);

	/// 读取并解析配置文件
	static IniConfig loadFile(const std::string &filename);

	/// 将配置写入到配置文件中
	static void write(IniConfig &iniConfig, const std::string &filename);

	virtual ~IniParse() {}

private:
	/// 判断是否是配置文件节名称
	static bool isSection(const std::string &str);

	/// 解析节的名称
	static std::string parseSection(const std::string &str);

	/// 判断是否是配置项
	static bool isOption(const std::string &str);

	/// 解析配置项
	static std::pair<std::string, std::string> parseOption(const std::string &str);

	/// 判断是否是配置文件中的注释
	static bool isComment(const std::string &str);

	/// 判断是否是空行
	static bool isEmptyLine(const std::string &str);

	/// 创建配置文件的节点
	static std::string createSection(const std::string &section);

	/// 创建配置文件的配置项
	static std::string createOption(const std::string &optionName, const std::string &optionValue);
};

class IniParseReadException : public std::exception {
public:
	IniParseReadException(const char *msg) : message(msg) {}
	virtual ~IniParseReadException() throw() {}
	virtual const char *what() const throw() { return message; }

private:
	const char *message;
};

class IniParseWriteException : public std::exception {
public:
	IniParseWriteException(const char *msg) : message(msg) {}
	virtual ~IniParseWriteException() throw() {}
	virtual const char *what() const throw() { return message; }

private:
	const char *message;
};

/*@}*/

}

#endif
