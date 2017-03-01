#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
    std::string filename = "/proc/self/status";
    std::string status;
    adbase::readFile(filename, 65536, &status);
    LOG_INFO << status;

    std::string status1;
    adbase::ReadSmallFile readfile(filename);
    readfile.readToString(65536, &status1, nullptr, nullptr, nullptr);
    LOG_INFO << status;

    adbase::ReadSmallFile readfileBuffer(filename);
    int size = 0;
    readfileBuffer.readToBuffer(&size);
    const char* buffer = readfileBuffer.buffer();
    LOG_INFO << "read file size:" << size;
    LOG_INFO << "read file content:" << buffer;

    return 0;
}
