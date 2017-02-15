#include <adbase/Config.hpp>
#include <adbase/Utility.hpp>
#include <fstream>
#include <iostream>

namespace adbase {
// {{{ IniConfig IniParse::load()

IniConfig IniParse::load(const std::string &iniStr) {
	std::vector<std::string> lines = explode(iniStr, '\n');
	IniConfig config;
	std::string section = "";
	for (auto &line : lines) {
		if (isSection(line)) {
			section = parseSection(line);
			config.addSection(section);
		} else if (isOption(line)) {
			std::pair<std::string, std::string> option = parseOption(line);
			config.addOption(section, option.first, option.second);
		} else if (isEmptyLine(line) || isComment(line)) {
			// ignore it
		} else {
			std::string msg = "Invalid line: " + line;
			throw IniParseReadException(msg.c_str());
		}
	}

	return config;
}

// }}}
// {{{ IniConfig IniParse::loadFile()

IniConfig IniParse::loadFile(const std::string &filename) {
	std::string result;
	readFile(filename, 65536, &result);
	return load(result);
}

// }}}
// {{{ void IniParse::write()

void IniParse::write(IniConfig &config, const std::string &filename) {
	std::ofstream os;
	os.open(filename.c_str());
	if (!os.is_open()) {
		std::string msg = "Unable to write " + filename;
		throw IniParseWriteException(msg.c_str());
	}

	try {
		std::vector<std::string> sections = config.sections();
		std::vector<std::string>::const_iterator s = sections.begin();
		for (; s != sections.end(); ++s) {
			os << createSection(*s) << std::endl;
			std::vector<std::string> opts = config.options(*s);
			std::vector<std::string>::const_iterator o = opts.begin();
			for (; o != opts.end(); ++o) {
				std::string value = config.getOption(*s, *o);
				os << createOption(*o, value) << std::endl;
			}
			os << std::endl;
		}
	} catch (...) {
		os.close();
		throw;
	}
}

// }}}
// {{{ bool IniParse::isSection()

bool IniParse::isSection(const std::string &str) {
	std::string trimmedStr = trim(str);
	if (trimmedStr[0] == '[' && trimmedStr[trimmedStr.size() - 1] == ']') {
		return true;
	}

	return false;
}

// }}}
// {{{ std::string IniParse::parseSection()

std::string IniParse::parseSection(const std::string &str) {
	std::string trimmedStr = trim(str);
	return trimmedStr.substr(1, trimmedStr.size() - 2);
}

// }}}
// {{{ bool IniParse::isOption()

bool IniParse::isOption(const std::string &str) {
	std::string trimmedStr = trim(str);
	std::string::size_type s = trimmedStr.find_first_of("=");
	if (s == std::string::npos) {
		return false;
	}
	std::string key = trim(trimmedStr.substr(0, s));
	if (key == ""){
		return false;	
	}

	return true;
}

// }}}
// {{{ std::pair<std::string, std::string> IniParse::parseOption()

std::pair<std::string, std::string> IniParse::parseOption(const std::string &str) {
	std::string trimmedStr = trim(str);
	std::string::size_type s = trimmedStr.find_first_of("=");
	std::string key = trim(trimmedStr.substr(0, s));
	std::string value = trim(trimmedStr.substr(s + 1));

	return std::pair<std::string, std::string>(key, value);
}

// }}}
// {{{ bool IniParse::isComment()

bool IniParse::isComment(const std::string &str) {
	std::string trimmedStr = leftTrim(str);
	if (trimmedStr[0] == ';') {
		return true;
	}

	return false;
}

// }}}
// {{{ bool IniParse::isEmptyLine()

bool IniParse::isEmptyLine(const std::string &str) {
	std::string trimmedStr = leftTrim(str);
	if (trimmedStr == "") {
		return true;
	}

	return false;
}

// }}}
// {{{ std::string IniParse::createSection()

std::string IniParse::createSection(const std::string &section) {
	return "[" + section + "]";
}

// }}}
// {{{ std::string IniParse::createOption()

std::string IniParse::createOption(const std::string &optionName, const std::string &optionValue) {
	return optionName + "=" + optionValue;
}

// }}}
}
