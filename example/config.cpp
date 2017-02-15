#include <adbase/Config.hpp>
#include <adbase/Logging.hpp>

int main() {
	std::string ini = "[http]\nhost=127.0.0.1\nport=80\ntimeout=3\ndaemon=yes\n";
	adbase::IniConfig config = adbase::IniParse::load(ini);
	LOG_INFO << config.getOption("http", "host");
	LOG_INFO << config.getOptionUint32("http", "port");
	LOG_INFO << config.getOptionUint32("http", "timeout");
	LOG_INFO << config.getOptionBool("http", "daemon");
	return 0;
}
