#include <adbase/Utility.hpp>
#include <gtest/gtest.h>
#include <random>

std::string generate_str(int str_len)
{
    std::string result;
    for (int i = 0; i < str_len; i++) {
        char ch = static_cast<char>('a' + rand() % 26);
        result.push_back(ch);
    }
    return result;
}

// {{{ construct
TEST(ReadSmallFileTest, ReadSmallFileConstructorTest)
{
    //create a regular file.
    char filename[]= "template-XXXXXX";
    int fd = mkstemp(filename);
    adbase::ReadSmallFile read_file(filename);
    close(fd);
    remove(filename);
}
// }}}

// {{{ read
TEST(ReadSmallFileTest, ReadFileTest)
{
    char filename[]= "template-XXXXXX";
    int str_len = 65535;
    std::string str_to_write = generate_str(str_len);

    //create a regular file.
    int fd = mkstemp(filename);

    //write some chars to fhe file
    ssize_t write_count = write (fd, str_to_write.c_str(), (str_to_write.size()));
    close(fd);
    EXPECT_EQ(write_count,str_to_write.size());

    //readTobuffer
    adbase::ReadSmallFile read_file(filename);
    int read_count = 0;
    read_file.readToBuffer(&read_count);
    EXPECT_EQ(read_count,write_count);
    EXPECT_EQ(std::string(read_file.buffer()),std::string(str_to_write.c_str()));

    //readToString
    int64_t file_size = 0;
    int64_t modify_time = 0;
    int64_t create_time = 0;
    int max_read = 65535;
    std::string result;
    adbase::readFile(filename,max_read, &result,&file_size,&modify_time,&create_time);
    EXPECT_EQ(result,str_to_write);

    //remove file
    remove(filename);
}
// }}}

// {{{ append
TEST(AppendFileTest, appendTest)
{
    char filename[]= "template-XXXXXX";
    int str_len = 65535;
    std::string str_to_write = generate_str(str_len);

    //create a regular file.
    int fd = mkstemp(filename);

    //write to file
    ssize_t write_count = write (fd, str_to_write.c_str(), (str_to_write.size()));
    close(fd);
    EXPECT_EQ(write_count,str_to_write.size());

    //append to file
    adbase::AppendFile append_file(filename);
    append_file.append(str_to_write.c_str(),str_to_write.size());
    append_file.flush();

    //read and compare
    int64_t file_size = 0;
    int64_t modify_time = 0;
    int64_t create_time = 0;
    int max_read = 65535*2;
    std::string result;
    adbase::readFile(filename,max_read, &result,&file_size,&modify_time,&create_time);
    EXPECT_EQ(result,str_to_write + str_to_write);
    remove(filename);
}
// }}}
