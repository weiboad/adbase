#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

std::string debug64(uint64_t value) {
    uint64_t base = 0x01;
    std::string str = "";
    for (int i = 63; i >= 0; i--) {
        str += (value & (base << i)) ? "1 " : "0 ";
    }

    return str;
}
std::string debug32(uint64_t value) {
    uint64_t base = 0x01;
    std::string str = "";
    for (int i = 31; i >= 0; i--) {
        str += (value & (base << i)) ? "1 " : "0 ";
    }

    return str;
}
std::string debug16(uint64_t value) {
    uint64_t base = 0x01;
    std::string str = "";
    for (int i = 15; i >= 0; i--) {
        str += (value & (base << i)) ? "1 " : "0 ";
    }

    return str;
}

int main() {
    uint64_t value = 12345;
    LOG_INFO << "before convert network enbian:" << debug64(value);
    uint64_t valuen = adbase::hostToNetwork64(value);
    LOG_INFO << "after convert network enbian:" << debug64(valuen);
    uint64_t valueh = adbase::networkToHost64(valuen);
    LOG_INFO << "reconvert network enbian:" << debug64(valueh);

    uint32_t value1 = 12345;
    LOG_INFO << "before convert network enbian:" << debug32(value1);
    uint32_t value1n = adbase::hostToNetwork32(value1);
    LOG_INFO << "after convert network enbian:" << debug32(value1n);
    uint32_t value1h = adbase::networkToHost32(value1n);
    LOG_INFO << "reconvert network enbian:" << debug32(value1h);

    uint16_t value2 = 12345;
    LOG_INFO << "before convert network enbian:" << debug16(value2);
    uint16_t value2n = adbase::hostToNetwork16(value2);
    LOG_INFO << "after convert network enbian:" << debug16(value2n);
    uint16_t value2h = adbase::networkToHost16(value2n);
    LOG_INFO << "reconvert network enbian:" << debug16(value2h);
    return 0;
}
