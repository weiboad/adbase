#include <adbase/Utility.hpp>
#include <gtest/gtest.h>
#include <random>
#include <dirent.h>

std::string generate_str(int str_len)
{
    std::string result;
    for (int i = 0; i < str_len; i++) {
        srand( static_cast<unsigned>(time(0)) );
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

// {{{ mkdirRecursive
TEST(FileSystemTest, mkdirRecursiveTest)
{
    std::string dir_path = std::string("/tmp/ad_base/") + generate_str(5);
    adbase::mkdirRecursive(dir_path, 755, true);
    DIR * p_dir = opendir(dir_path.c_str());
    EXPECT_EQ( p_dir != NULL,true);
    EXPECT_EQ(closedir(p_dir),0);
    EXPECT_EQ(remove(dir_path.c_str()),0);
}
// }}}

// {{{ dirRecursive
TEST(FileSystemTest, dirRecursiveTest)
{
    std::string dir_path = std::string("/tmp/ad_base/") ;
    adbase::mkdirRecursive(dir_path,755, true);
    DIR * p_dir = opendir(dir_path.c_str());
    EXPECT_EQ( p_dir != NULL,true);
    EXPECT_EQ(closedir(p_dir),0);

    std::string filename = generate_str(5);
    creat(std::string(dir_path + filename).c_str(),755);
    std::vector<std::string> path_info;
    std::vector<std::string> excludes;
    adbase::recursiveDir(dir_path, true,excludes,path_info);

    EXPECT_EQ(path_info.size(),1);
    remove(std::string(dir_path + filename).c_str());
    remove(dir_path.c_str());
}
// }}}
