#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main(void) {
    // get pid
    LOG_INFO << "process id:" << adbase::pid();
    LOG_INFO << "process id:" << adbase::pidString();
    LOG_INFO << "process name:" << adbase::procname();

    return 0;
}
