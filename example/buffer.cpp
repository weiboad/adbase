#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

int main() {
    adbase::Buffer buffer;

    // write data 
    LOG_INFO << "Init readable length: " << buffer.readableBytes();
    LOG_INFO << "Init writable length: " << buffer.writableBytes();

    const char* c = "bar";
    buffer.append(c, 3);
    LOG_INFO << "input char* readable length: " << buffer.readableBytes();
    LOG_INFO << "input char* writable length: " << buffer.writableBytes();

    std::string str = "hello";
    buffer.append(str);
    LOG_INFO << "input std::string readable length: " << buffer.readableBytes();
    LOG_INFO << "input std::string writable length: " << buffer.writableBytes();

    int64_t int64 = 123;
    buffer.appendInt64(int64);
    LOG_INFO << "input int64_t readable length: " << buffer.readableBytes();
    LOG_INFO << "input int64_t writable length: " << buffer.writableBytes();

    int32_t int32 = 123;
    buffer.appendInt32(int32);
    LOG_INFO << "input int32_t readable length: " << buffer.readableBytes();
    LOG_INFO << "input int32_t writable length: " << buffer.writableBytes();

    int16_t int16 = 123;
    buffer.appendInt16(int16);
    LOG_INFO << "input int16_t readable length: " << buffer.readableBytes();
    LOG_INFO << "input int16_t writable length: " << buffer.writableBytes();

    int8_t int8 = 123;
    buffer.appendInt8(int8);
    LOG_INFO << "input int8_t readable length: " << buffer.readableBytes();
    LOG_INFO << "input int8_t writable length: " << buffer.writableBytes();

    // read data
    LOG_INFO << std::string(buffer.peek(), 3);
    buffer.retrieve(3);
    LOG_INFO << "output char 3 readable length: " << buffer.readableBytes();
    LOG_INFO << "output char 3 writable length: " << buffer.writableBytes();

    LOG_INFO << std::string(buffer.peek(), 5);
    buffer.retrieve(5);
    LOG_INFO << "output char 5 readable length: " << buffer.readableBytes();
    LOG_INFO << "output char 5 writable length: " << buffer.writableBytes();

    LOG_INFO << buffer.readInt64();
    LOG_INFO << "output int64_t readable length: " << buffer.readableBytes();
    LOG_INFO << "output int64_t writable length: " << buffer.writableBytes();

    LOG_INFO << buffer.readInt32();
    LOG_INFO << "output int32_t readable length: " << buffer.readableBytes();
    LOG_INFO << "output int32_t writable length: " << buffer.writableBytes();

    LOG_INFO << buffer.readInt16();
    LOG_INFO << "output int16_t readable length: " << buffer.readableBytes();
    LOG_INFO << "output int16_t writable length: " << buffer.writableBytes();

    LOG_INFO << buffer.readInt8();
    LOG_INFO << "output int8_t readable length: " << buffer.readableBytes();
    LOG_INFO << "output int8_t writable length: " << buffer.writableBytes();
    return 0;
}
