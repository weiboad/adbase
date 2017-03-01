#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
    LOG_INFO << "proc open file:" << adbase::procFdNum();
    LOG_INFO << "proc status:";

    std::unordered_map<std::string, std::string> status = adbase::procStats();
    for (auto &t : status) {
        LOG_INFO << "   " << t.first << ":" << t.second;
    }
    return 0;
}
