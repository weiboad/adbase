#include <adbase/Logging.hpp>

int main() {
	// 设置日志级别
	adbase::Logger::setLogLevel(adbase::Logger::TRACE);

    // input logging
	LOG_TRACE << "trace test";
	LOG_DEBUG << "debug test";
	LOG_INFO  << "info test";
	LOG_ERROR << "error test";

    // input pointer
    int* p;
    int a = 5;
    p = &a; 
    LOG_ERROR << p;

	// 格式化的方式写入
	LOG_INFO  << adbase::Fmt("fmt %d test", 45);

	return 0;
}
