#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <adbase/Config.hpp>
#include "Seed.hpp"

// {{{ Seed::Seed()

Seed::Seed(const std::string config, const std::string input, const std::string output) :
	_configFile(config),
	_inputDir(input),
	_outputDir(output) {
}

// }}}
// {{{ Seed::~Seed()

Seed::~Seed() {

}

// }}}
// {{{ void Seed::init()

void Seed::init() {
	adbase::IniConfig config = adbase::IniParse::loadFile(_configFile);
	std::vector<std::string> projectParams = config.options("project");
	for (auto &t : projectParams) {
		_projectParams[t] = config.getOption("project", t);		
	}
	std::vector<std::string> modules = config.options("module");
	for (auto &t : modules) {
		if (config.getOptionBool("module", t)) {
			_modules[t] = true;	
		}
	}
	std::vector<std::string> replaceFiles = config.options("files");
	for (auto &t : replaceFiles) {
		_replaceFiles[t] = config.getOption("files", t);
	}
	std::vector<std::string> params = config.options("params");
	for (auto &t : params) {
		_params[t] = adbase::explode(config.getOption("params", t), ',', true);
	}
	std::vector<std::string> chmods = config.options("execs");
	for (auto &t : chmods) {
		_chmodFiles[t] = true;
	}
}

// }}}
// {{{ bool Seed::isIfStart()

/**
 * //@IF (@mc || @head) 
 * 判断是否是 IF 标记开始
 * 
 * @param  mixed $line 
 * @return void
 */
bool Seed::isIfStart(const std::string& line) {
	std::vector<std::string> lines = adbase::explode(adbase::trim(line), ' ', true);
	if (lines.size() >= 2 && adbase::trim(lines[0]).compare("//@IF") == 0) {
		return true;
	} else {
		return false;
	}
}

// }}}
// {{{ bool Seed::isIfEnd()

/**
 * //@ENDIF
 * 
 * @param  mixed $line 
 * @return void
 */
bool Seed::isIfEnd(const std::string& line) {
	if (adbase::trim(line).compare("//@ENDIF") == 0) {
		return true;
	}
	return false;
}

// }}}
// {{{ bool Seed::parseIf()

bool Seed::parseIf(const std::string& input) {
	std::vector<std::string> lines = adbase::explode(adbase::trim(input), ' ', false);
	std::string ifInput = "";
	for (auto iter = lines.begin() + 1; iter != lines.end(); iter++) {
		ifInput += " " + *(iter);
	}
	return parseIfCondtion(ifInput);
}

// }}}
// {{{ bool Seed::parseIfCondtion()

bool Seed::parseIfCondtion(const std::string& input) {
	std::string ifStr = adbase::trim(input);
	if (ifStr.size() < 1) {
		LOG_SYSFATAL << "Condition is not valid.";		
	}
	if (ifStr[0] != '!' && ifStr[0] != '@' && ifStr[0] != '(') {
		LOG_SYSFATAL << "Condition is not valid.";		
	}
	std::vector<IfResult> opResult;
	// {{{ parse
	std::vector<char> stackIf;
	std::vector<char> stackCondition;
	bool isIf = false; // 是否是嵌套 if 语句
	bool isCondition = false; // 是否是 if 条件 
	bool isNot = false; // 是否是 not 条件
	int ifnum  = 0; // 嵌套层级
	size_t length = ifStr.size();
	int ornum = 0; // | 操作表达式个数
	int andnum = 0; // & 操作表达式个数
	for (size_t i = 0; i < length; i++) {
		bool isEnd = (i == (length - 1));
		char currentChar = ifStr[i];
		if (currentChar == '!') {
			isNot = true;
			continue;
		}
		// {{{ || &&
		if (currentChar == '|' && !isCondition && !isIf) {
			ornum++;
			if (ornum > 2) {
				LOG_SYSFATAL << "|| 操作符不对";
			}

			if (ornum == 2) {
				ornum = 0;	
				IfResult item;
				item.type = IF_OR;
				opResult.push_back(item);
			}
			continue;
		}
		if (currentChar == '&' && !isIf && !isCondition) {
			andnum++;	
			if (andnum > 2) {
				LOG_SYSFATAL << "&& 操作符不对";
			}

			if (andnum == 2) {
				andnum = 0;	
				IfResult item;
				item.type = IF_AND;
				opResult.push_back(item);
			}
			continue;
		}

		if (andnum || ornum) {
			LOG_SYSFATAL << "表达式错误存在 & |";
		}
		// }}}
		// {{{ condition
		if (currentChar == '@' && !isIf) {
			isCondition = true;
			continue;
		}

		if (isCondition) {
			if (currentChar == ' ' || isEnd) {
				isCondition = false;
				if (isEnd) {
					stackCondition.push_back(currentChar);
				}
				IfResult item;
				item.type = IF_CONDITION;
				item.data = std::string(&*stackCondition.begin(), stackCondition.size());
				if (isNot) {
					item.isNot = true;
					isNot = false;
				} else {
					item.isNot = false;
				}
				opResult.push_back(item);
				stackCondition.clear();
				continue;
			} 
			
			if ((currentChar >='A' && currentChar <= 'Z')
						 || (currentChar >= 'a' && currentChar <= 'z')) {
				stackCondition.push_back(currentChar);	
			} else {
				LOG_SYSFATAL << "condition name is invalid.";	
			}
			continue;
		}

		// }}}
		// {{{ if
		if (currentChar == '(' && !isIf) {
			isIf = true;
			ifnum++;
			continue;
		}

		if (currentChar == '(' && isIf) {
			ifnum++;
		}

		if (isIf) {
			if (currentChar == ')') {
				ifnum--;
				if (ifnum == 0) {
					IfResult item;
					item.type = IF_IF;
					item.data = std::string(&*stackIf.begin(), stackIf.size());
					isIf = false;
					stackIf.clear();
					continue;
				}
			}
			stackIf.push_back(currentChar);
			continue;
		}

		// }}}
		if (currentChar == ' ') {
			continue;
		}
		LOG_SYSFATAL << "not run ...char:" << currentChar;
	}
	// }}}
	if (opResult.empty() || opResult[0].type == IF_OR || opResult[0].type == IF_AND) {
		LOG_SYSFATAL << "expr is invalid";
	}

	//for (auto &t : opResult) {
	//	std::string typeStr = "";
	//	if (t.type == IF_AND) {
	//		typeStr = "and";
	//	} else if (t.type == IF_OR) {
	//		typeStr = "or";	
	//	} else if (t.type == IF_IF) {
	//		typeStr = "if";
	//	} else if (t.type == IF_CONDITION) {
	//		typeStr = "condition";
	//	}
	//	LOG_INFO << "type:" << typeStr << " data:" << t.data << " isNot:" << t.isNot;
	//}

	bool result;
	IfResult item = opResult[0];
	if (item.type == IF_CONDITION) {
		if (item.isNot) {
			result = (_modules.find(item.data) == _modules.end());
		} else {
			result = (_modules.find(item.data) != _modules.end());
		}
	} else {
		result = parseIfCondtion(item.data);
	}

	if (opResult.size() == 1) {
		return result;
	}
	IfType currentOp = IF_OR;
	bool prevIsOp = false; // 上一个是操作符
	for (auto iter = opResult.begin() + 1; iter != opResult.end(); iter++) {
		if ((iter->type == IF_OR || iter->type == IF_AND) && !prevIsOp) {
			currentOp = iter->type;	
			prevIsOp  = true;
			continue;
		}
		if ((iter->type == IF_CONDITION || iter->type == IF_IF) && prevIsOp) {
			//false && return false.....
			//true || return true.....
			if (result && currentOp == IF_OR) {
				return true;
			} else if (!result && currentOp == IF_AND) {
				return false;
			} else {
				if (iter->type == IF_CONDITION) {
					if (iter->isNot) {
						result = (_modules.find(iter->data) == _modules.end());
					} else {
						result = (_modules.find(iter->data) != _modules.end());
					}
				} else {
					result = parseIfCondtion(iter->data);
				}
			}
			prevIsOp = false;
			continue;
		}
		LOG_SYSFATAL << "expr is invalid";
	}

	return result;
}

// }}}
// {{{ bool Seed::isForStart()

/**
 * //@FOR @mc, @head
 * 判断是否是 FOR 标记开始
 * 
 * @param  mixed $line 
 * @return void
 */
bool Seed::isForStart(const std::string& line) {
	std::vector<std::string> lines = adbase::explode(adbase::trim(line), ' ', true);
	if (lines.size() >= 2 && adbase::trim(lines[0]).compare("//@FOR") == 0) {
		return true;
	} else {
		return false;
	}
}

// }}}
// {{{ bool Seed::isForEnd()

/**
 * //@ENDFOR
 * 
 * @param  mixed $line 
 * @return void
 */
bool Seed::isForEnd(const std::string& line) {
	if (adbase::trim(line).compare("//@ENDFOR") == 0) {
		return true;
	}
	return false;
}

// }}}
// {{{  std::vector<std::string> Seed::parseFor()

/**
 * //@For @timers , @stats
 * @return void
 */
std::vector<std::string> Seed::parseFor(const std::string& input) {
	std::vector<std::string> forTags;
	std::vector<std::string> lines = adbase::explode(adbase::trim(input), ' ', false);
	std::string forInput = "";
	for (auto iter = lines.begin() + 1; iter != lines.end(); iter++) {
		forInput += " " + *(iter);
	}
	std::vector<std::string> values = adbase::explode(forInput, ',', true);
	for (auto &t : values) {
		std::string trimt = adbase::trim(t);
		if (trimt.substr(0, 1).compare("@") != 0) {
			LOG_WARN << "For condition:" << t << " is invalid, input:" << input;
			continue;
		}
		std::string tagName = trimt.substr(1);
		if (_params.find(tagName) == _params.end()) {	
			LOG_WARN << "For condition:" << t << " is not exists, input:" << input;
			continue;
		}
		forTags.push_back(tagName);	
	}
	return forTags;
}

// }}}
// {{{ std::vector<std::string> Seed::parseLineIf()

std::vector<std::string> Seed::parseLineIf(const std::vector<std::string>& input) {
	bool isIfBlock = false;
	bool ifCondition = true;
	int ifnum   = 0;
	std::vector<std::string> ifStack;
	bool currentIsContinue = false;
	std::vector<std::string> output;
	for (auto &line : input) {
		if (isIfStart(line)) {
			ifnum++;
			if (!isIfBlock) { // 不在 if 嵌套语句块中
				ifCondition = parseIf(line);
				isIfBlock = true;
				currentIsContinue = false;
				for (auto &t : ifStack) {
					output.push_back(t);
				}
				ifStack.clear();
			} else {
				currentIsContinue = true;
				if (ifCondition) {
					ifStack.push_back(line);
				}
			}
		} else if (isIfEnd(line)) {
			ifnum--;
			if (ifnum == 0) {
				isIfBlock = false;	
				ifCondition = true;
				if (ifStack.empty()) {
					continue;
				} else {
					if (currentIsContinue) {
						std::vector<std::string> ifOutput = parseLineIf(ifStack);
						for (auto &t: ifOutput) {
							output.push_back(t);
						}
					} else {
						for (auto &t: ifStack) {
							output.push_back(t);
						}
					}
					ifStack.clear();
				}
			} else {
				if (ifCondition) {
					ifStack.push_back(line);
				}
			}
		} else {
			if (ifCondition) {
				ifStack.push_back(line);
			}
		}
	}

	if (!ifStack.empty()) {
		for (auto &t: ifStack) {
			output.push_back(t);
		}
	}

	return output;
}

// }}}
// {{{ std::vector<std::string> Seed::parseLineFor()

std::vector<std::string> Seed::parseLineFor(const std::vector<std::string>& input) {
	bool isForBlock = false;
	int fornum   = 0;
	std::vector<std::string> forStack;
	bool currentIsContinue = false;
	std::vector<std::string> output;
	std::vector<std::string> forValues;
	for (auto &line : input) {
		if (isForStart(line)) {
			fornum++;
			if (!isForBlock) { // 不在 for 嵌套语句块中
				forValues = parseFor(line);
				isForBlock = true;
				currentIsContinue = false;
				for (auto &t : forStack) {
					output.push_back(t);	
				}
				forStack.clear();
			} else {
				currentIsContinue = true;
				forStack.push_back(line);
			}
		} else if (isForEnd(line)) {
			fornum--;
			if (fornum == 0) {
				isForBlock = false;	
				if (forStack.empty()) {
					continue;
				} else {
					if (currentIsContinue) {
						forStack = parseLineFor(forStack);
					}

					size_t maxReplaces = 0;
					for (auto &t: forValues) {
						if (maxReplaces < _params[t].size()) {
							maxReplaces = _params[t].size();	
						}
					}
					for(size_t i = 0; i < maxReplaces; i++) {
						for (auto &forval : forStack) {
							std::string tmp = forval;
							for(size_t j = 0; j < forValues.size(); j++) {
								int count;
								std::string searchStr = "@REPLACE" + std::to_string(j);
								std::string replaceStr = _params[forValues[j]][i];
								tmp = adbase::replace(searchStr + "@", replaceStr, tmp, count);
								std::string replaceStrLower = _params[forValues[j]][i];
								std::transform(replaceStrLower.begin(), replaceStrLower.end(), replaceStrLower.begin(), ::tolower);
								tmp = adbase::replace(searchStr + "|lower@", replaceStrLower, tmp, count);
								std::string replaceStrUcfirst = _params[forValues[j]][i];
								std::transform(replaceStrUcfirst.begin(), replaceStrUcfirst.begin() + 1, replaceStrUcfirst.begin(), ::toupper);
								tmp = adbase::replace(searchStr + "|ucfirst@", replaceStrUcfirst, tmp, count);
								std::string replaceStrUpper = _params[forValues[j]][i];
								std::transform(replaceStrUpper.begin(), replaceStrUpper.end(), replaceStrUpper.begin(), ::toupper);
								tmp = adbase::replace(searchStr + "|upper@", replaceStrUpper, tmp, count);
							}
							output.push_back(tmp);
						}
					}
					forValues.clear();
					forStack.clear();
				}
			} else {
				forStack.push_back(line);
			}
		} else {
			forStack.push_back(line);
		}
	}

	if (!forStack.empty()) {
		for (auto &t : forStack) {
			output.push_back(t);
		}
	}

	return output;
}

// }}}
// {{{ std::vector<std::string> Seed::macroReplace()

std::vector<std::string> Seed::macroReplace(const std::vector<std::string>& input) {
	std::vector<std::string> result;
	for (auto &t : input) {
		std::string tmp = t;
		for (auto &m : _projectParams) {
			std::string searchStr = "@" + m.first;	
			int count;
			tmp = adbase::replace(searchStr + "@", m.second, tmp, count);
			std::string replaceStrLower = m.second;
			std::transform(replaceStrLower.begin(), replaceStrLower.end(), replaceStrLower.begin(), ::tolower);
			tmp = adbase::replace(searchStr + "|lower@", replaceStrLower, tmp, count);
			std::string replaceStrUcfirst = m.second;
			std::transform(replaceStrUcfirst.begin(), replaceStrUcfirst.begin() + 1, replaceStrUcfirst.begin(), ::toupper);
			tmp = adbase::replace(searchStr + "|ucfirst@", replaceStrUcfirst, tmp, count);
			std::string replaceStrUpper = m.second;
			std::transform(replaceStrUpper.begin(), replaceStrUpper.end(), replaceStrUpper.begin(), ::toupper);
			tmp = adbase::replace(searchStr + "|upper@", replaceStrUpper, tmp, count);
		}
		result.push_back(tmp);
	}

	return result;
}

// }}}
// {{{ void Seed::writeReplaceFile()

void Seed::writeReplaceFile(const std::string& filename, const std::vector<std::string>& replaceParams, const std::vector<std::string>& lines) {
	if (replaceParams.size() != 1) {
		LOG_SYSFATAL << "-@xxx- must count 1";
	}

	std::vector<std::string> replaceNames = _params[replaceParams[0]];
	int count;
	std::string replaceFileName = adbase::replace("-", "", filename, count);
	for (auto &t : replaceNames) {
		std::string tmpFileName= replaceFileName;
		tmpFileName = adbase::replace("@" + replaceParams[0], t, tmpFileName, count);	
		std::vector<std::string> linesResult;
		std::string searchStr = "@FILEREPLACE0";	
		for (auto &l : lines) {
			std::string tmp = l;
			int count;
			tmp = adbase::replace(searchStr + "@", t, tmp, count);
			std::string replaceStrLower = t;
			std::transform(replaceStrLower.begin(), replaceStrLower.end(), replaceStrLower.begin(), ::tolower);
			tmp = adbase::replace(searchStr + "|lower@", replaceStrLower, tmp, count);
			std::string replaceStrUcfirst = t;
			std::transform(replaceStrUcfirst.begin(), replaceStrUcfirst.begin() + 1, replaceStrUcfirst.begin(), ::toupper);
			tmp = adbase::replace(searchStr + "|ucfirst@", replaceStrUcfirst, tmp, count);
			std::string replaceStrUpper = t;
			std::transform(replaceStrUpper.begin(), replaceStrUpper.end(), replaceStrUpper.begin(), ::toupper);
			tmp = adbase::replace(searchStr + "|upper@", replaceStrUpper, tmp, count);
			linesResult.push_back(tmp);
		}
		writeCommonFile(tmpFileName, linesResult);
	}
}

// }}}
// {{{ void Seed::writeCommonFile()

void Seed::writeCommonFile(const std::string& filename, const std::vector<std::string>& lines) {
	int count;
	std::string outname = adbase::replace(_inputDir, _outputDir, filename, count);
	std::string fileBaseName = adbase::leftTrim(adbase::replace(_outputDir, "", outname, count), "/");
	fileBaseName = fileBaseName.substr(0, fileBaseName.size() - 2);
	if (_replaceFiles.find(fileBaseName) != _replaceFiles.end()) {
		std::string tmpReplace = _replaceFiles[fileBaseName];
		for (auto &t : _projectParams) {
			tmpReplace = adbase::replace("@" + t.first + "@", t.second, tmpReplace, count);					
		}
		fileBaseName = tmpReplace;
	}

	std::string outputName = adbase::rightTrim(_outputDir, "/") + "/" + fileBaseName;
	std::string output = "";
	for (auto &t : lines) {
		output += t;	
		output += "\n";
	}
	output = adbase::trim(output);

	if (output.empty()) {
		return;	
	}

	std::string name = outputName.substr(outputName.find_last_of("/") + 1);
	std::string dirname = adbase::replace(name, "", outputName, count); 
	adbase::mkdirRecursive(dirname, 0755, true);
	adbase::AppendFile appendFile(outputName);
	if (_chmodFiles.find(name) != _chmodFiles.end()) {
		chmod(outputName.c_str(), 0755);
	}
	appendFile.append(output.c_str(), output.size());
	std::cout << "\t" <<  outputName << std::endl;
}

// }}}
// {{{ void Seed::parseFile()

void Seed::parseFile(const std::string& filename) {
	std::string fileInput;
	adbase::readFile(filename, 65536, &fileInput);
	std::vector<std::string> lines = adbase::explode(fileInput, '\n', false);
	lines = parseLineIf(lines);
	lines = parseLineFor(lines);
	lines = macroReplace(lines);
	std::vector<std::string> result;
	size_t lineNum = lines.size();
	for (size_t i = 0; i < lineNum - 1; i++) {
		if (adbase::trim(lines[i]).empty() 
			&& adbase::trim(lines[i + 1]).empty()) {
			continue;
		}
		result.push_back(lines[i]);
	}

	std::string fileBaseName = filename.substr(filename.find_last_of("/") + 1);
	std::vector<std::string> splitFilenames = adbase::explode(fileBaseName, '-', true);
	std::vector<std::string> replaceWords;
	bool isReplaceFile = false;
	for (auto &split : splitFilenames) {
		if (split.substr(0, 1).compare("@") == 0) {
			if (_params.find(split.substr(1)) != _params.end()) {
				replaceWords.push_back(split.substr(1));	
				isReplaceFile = true;
			}
		}
	}

	if (isReplaceFile) {
		writeReplaceFile(filename, replaceWords, result);
	} else {
		writeCommonFile(filename, result);
	}
}

// }}}
// {{{ void Seed::run()

void Seed::run() {
	try {
		init();	
	} catch (std::exception &e) {
        LOG_SYSFATAL << "Main exception: " << e.what();	
	}

	std::vector<std::string> excludes;
	std::vector<std::string> paths;
	adbase::recursiveDir(_inputDir, true, excludes, paths);
	std::cout << "Generate file list:" << std::endl;
	for (auto &t : paths) {
		if (t.substr(t.size() -2, 2).compare(".c") != 0) {
			continue;
		}
		parseFile(t);
	}

}

// }}}
// {{{ void usage()

void usage() {
	std::cout << "Usage: adbase_seed [options...] <path>" << std::endl;
	std::cout << "\t-c: 主配置文件路径" << std::endl;
	std::cout << "\t-i: 骨架模板目录" << std::endl;
	std::cout << "\t-d: 生成骨架目录" << std::endl;
	std::cout << "\t-h: 帮助" << std::endl;
	exit(0);
} 

// }}}
// {{{ int main()

int main(int argc, char** argv) {
	int ch;
	std::string configFile = "./seed.ini";
	std::string inputDir = "/var/lib/adbase/";
	std::string outputDir = "./";
	while((ch = getopt(argc, argv, "c:i:d:h")) != -1) {
		if (ch == 'c') {
			configFile = optarg;
		} else if (ch == 'i') {
			inputDir = optarg;
		} else if (ch == 'd') {
			outputDir = optarg;
			if (outputDir.compare(".") == 0) {
				outputDir = "./";
			}
		} else {
			usage();
		}
	}

	LOG_INFO  << outputDir;
	Seed seed(configFile, inputDir, outputDir);
	seed.run();
	return 0;
}

// }}}
