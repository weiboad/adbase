#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main() {

    /// base64 encode/decode example
    std::string base64 = "test base64";
    std::string base64encode = adbase::base64Encode(base64.c_str(), base64.size());
    LOG_INFO << base64encode;
    char base64decode[base64encode.size()];
    memset(base64decode, 0, base64encode.size());
    int length = 0;
    adbase::base64Decode(base64decode, &length, base64encode);
    LOG_INFO << base64decode;
    return 0;
}
