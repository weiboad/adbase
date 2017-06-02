#include <adbase/Logging.hpp>
#include <unordered_map>

void printMallInfo() {
    std::unordered_map<std::string, int> info = adbase::mallInfo();
    for (auto &t : info) {
        LOG_INFO << t.first << ":" << t.second;
    }
}

int main(void) {
    std::unordered_map<std::string, std::string> data;

    printMallInfo();
    for (int i = 0; i < 100000; i++) {
        std::string key = "key" + std::to_string(i);
        std::string value = key + std::to_string(i);

        data[key] = value;
    }
    printMallInfo();
    for (int i = 0; i < 100000; i++) {
        std::string key = "key" + std::to_string(i);
        data.erase(key);
    }
    printMallInfo();
    adbase::mallocTrim(128 * 1024); 
    printMallInfo();


    while(1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
    }
    return 0;
}
