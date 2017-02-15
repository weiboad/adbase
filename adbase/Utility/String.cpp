#include <adbase/Utility.hpp>

namespace adbase {
// {{{ trim
const std::string TRIM_DELIMITERS = " \f\n\r\t\v";
std::string rightTrim(const std::string &str, const char* trimStr) {
	std::string::size_type s;
	if (trimStr == nullptr) {
		s = str.find_last_not_of(TRIM_DELIMITERS);
	} else {
		s = str.find_last_not_of(trimStr);
	}
    if (s == std::string::npos) {
        return "";
    }

    return str.substr(0, s + 1);
}

std::string leftTrim(const std::string &str, const char* trimStr) {
	std::string::size_type s;
	if (trimStr == nullptr) {
		s = str.find_first_not_of(TRIM_DELIMITERS);
	} else {
		s = str.find_first_not_of(trimStr);
	}
    if (s == std::string::npos) {
        return "";
    }

    return str.substr(s);
}

std::string trim(const std::string &str, const char* trimStr) {
    return rightTrim(leftTrim(str, trimStr), trimStr);
}

// }}}
// {{{ const std::vector<std::string> explode()

const std::vector<std::string> explode(const std::string& s, const char& c, bool isTrim) {
    std::string buff{""};
    std::vector<std::string> v;

	char last = 0;
    for(auto n : s) {
        if (n != c) {
            buff += n;
        } else if ((n == c && buff != "") || c == last) {
            if (isTrim) {
                buff = trim(buff);
            }
            v.push_back(buff); buff = "";
        }
		last = n;
    }

    if (buff != "" || last == c) {
        v.push_back(buff);
    }

    return v;
}

// }}}
// {{{ const std::string& replace()

const std::string replace(const std::string& search,
						   const std::string& replace,
						   const std::string& subject,
						   int& count) {
	std::string::size_type start = 0;
	std::string::size_type found = std::string::npos;
	std::string result;
	count = 0;
	do {
		found = subject.find(search, start);
		if (found != std::string::npos) {
			result.append(subject.substr(start, found - start));
			result.append(replace);
			count++;
		} else {
			// 添加后半段
			result.append(subject.substr(start));
		}
		start = found + search.size();
	} while (found != std::string::npos);

	return result;
}

// }}}
}
