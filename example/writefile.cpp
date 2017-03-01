#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
    std::string filename = "./bar";
    adbase::AppendFile fp(filename);
    std::string data = "test";
    fp.append(data.c_str(), data.size());
    fp.flush();
    LOG_INFO << "write file size:" << fp.writtenBytes();

    return 0;
}
