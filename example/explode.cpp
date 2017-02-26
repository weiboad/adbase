#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
	std::string subject = "a ab a  ba";

	std::vector<std::string> result = adbase::explode(subject, 'a', false);
	LOG_INFO << "result count:" << result.size();
	for(auto &t : result) {
		LOG_INFO << "char:" << t << " |size|:" << t.size();
	}

	std::vector<std::string> trimResult = adbase::explode(subject, 'a', true);
	LOG_INFO << "trim result count:" << trimResult.size();
	for(auto &t : trimResult) {
		LOG_INFO << "trim char:" << t << " |size|:" << t.size();
	}
	return 0;
}
