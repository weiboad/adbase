#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
    LOG_INFO << "host name:" << adbase::hostname();
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> ifconfigs = adbase::ifconfig();
    for (auto &t : ifconfigs) {
        LOG_INFO << "Network name:" << t.first; 
        for (auto &info : t.second) {
            LOG_INFO << "info->" << info.first << ":" << info.second; 
        }
    }
    return 0;
}
