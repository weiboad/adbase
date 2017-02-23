#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main() {
    adbase::Buffer buffer;

    // write data 
    
    LOG_INFO << "Init readable length: " << buffer.readableBytes();
    LOG_INFO << "Init readable length: " << buffer.writableBytes();
    return 0;
}
