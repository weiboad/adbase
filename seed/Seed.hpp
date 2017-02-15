#ifndef ADBASE_SEED_HPP_ 
#define ADBASE_SEED_HPP_ 

class Seed {
public:
	Seed(const std::string config, const std::string input, const std::string output);
	~Seed();
	void run();

private:
	std::string _configFile;
	std::string _inputDir;
	std::string _outputDir;
	std::unordered_map<std::string, std::string> _projectParams;
	std::unordered_map<std::string, bool> _modules;
	std::unordered_map<std::string, std::string> _replaceFiles;
	std::unordered_map<std::string, std::vector<std::string>> _params;
	std::unordered_map<std::string, bool> _chmodFiles; // 755
	void init();
	bool isIfStart(const std::string& line);
	bool isIfEnd(const std::string& line);
	bool parseIfCondtion(const std::string& input);
	bool parseIf(const std::string& input);
	bool isForStart(const std::string& line);
	bool isForEnd(const std::string& line);
	std::vector<std::string> parseFor(const std::string& input);
	std::vector<std::string> parseLineFor(const std::vector<std::string>& input);
	std::vector<std::string> parseLineIf(const std::vector<std::string>& input);
	std::vector<std::string> macroReplace(const std::vector<std::string>& input);
	void writeReplaceFile(const std::string& filename, const std::vector<std::string>& replaceParams, const std::vector<std::string>& lines);
	void writeCommonFile(const std::string& filename, const std::vector<std::string>& lines);
	void parseFile(const std::string& filename);
};

typedef enum ifType {
	IF_OR = 0,
	IF_AND = 1,
	IF_IF = 2,
	IF_CONDITION = 3,
} IfType;

typedef struct ifResult {
	IfType type;
	std::string	data;
	bool isNot;
} IfResult;
#endif
