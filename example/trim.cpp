#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
	std::string str = "\n\t\vtest\n   ";

	LOG_INFO << "string len:" << str.size();	
	LOG_INFO << "left trim after len: " << adbase::leftTrim(str).size();
	LOG_INFO << "right trim after len: " << adbase::rightTrim(str).size();
	LOG_INFO << "all trim after len: " << adbase::trim(str).size();
	
	std::string trimChar = "\n\v\t t";
	LOG_INFO << "custom left trim after len: " << adbase::leftTrim(str, trimChar.c_str()).size();
	LOG_INFO << "custom right trim after len: " << adbase::rightTrim(str, trimChar.c_str()).size();
	LOG_INFO << "custom all trim after len: " << adbase::trim(str, trimChar.c_str()).size();
	return 0;
}
