#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
	std::string subject = "aabbccaabbccbbccaa";
	int count = 0;
	
	LOG_INFO << adbase::replace("aa", "AA", subject, count);	
	LOG_INFO << "replace count:" << count;
	return 0;
}
