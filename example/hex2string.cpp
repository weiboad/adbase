#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main() {
    std::string hexStr = "efabca";
    char buffer[3] = {0};
    adbase::hexStringToBytes(hexStr, buffer);

    std::string result = adbase::bytesToHexString(buffer, 3);
	LOG_INFO << result;
    return 0;
}
