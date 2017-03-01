#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {

    // 目录路劲
    // 目录权限
    // 目录递归
    if (!adbase::mkdirRecursive("./bar/foo/aa/bb", 0755, true)) {
        LOG_INFO << "mkdir fail.";
        return 0;
    }

    std::vector<std::string> excludes;
    std::vector<std::string> dirs;
    adbase::recursiveDir("./", true, excludes, dirs);
    for (auto &t : dirs) {
        LOG_INFO << t;
    }
    return 0;
}
