#include <adbase/Utility.hpp>
#include <gtest/gtest.h>
#include <random>
#include <vector>

int random_int(int min,int max)
{
    std::default_random_engine random_engine;
    std::uniform_int_distribution<size_t> num_range(min, max);
    return static_cast<int>(num_range(random_engine));
}

// {{{ trim
TEST(StringTest, StringTrimTest) {

    const std::string TRIM_DELIMITERS = " \f\n\r\t\v";

    for (auto ch_trim : TRIM_DELIMITERS)
    {
        size_t invaild_chars_lenth = static_cast<size_t>(random_int(10000,20000));
        size_t vaild_chars_lenth = static_cast<size_t>(random_int(10000,20000));

        std::string chars_for_trim(invaild_chars_lenth,ch_trim);

        std::string valid_chars(vaild_chars_lenth,static_cast<char>('a'+rand()%26));
        std::string all_chars = chars_for_trim + valid_chars + chars_for_trim;

        all_chars = adbase::trim(all_chars,TRIM_DELIMITERS.c_str());

        EXPECT_EQ(all_chars, valid_chars);
    }

}
// }}}

// {{{ replace
TEST(StringTest, StringReplaceTest) {

    size_t num = static_cast<size_t>(random_int(10000,20000));
    std::string str;

    for(size_t i=0 ;i<num; i++)
    {
        str.push_back(static_cast<char>('a'+rand()%26));
        str.push_back('!');
    }

    int n_replace = 0;
    str = adbase::replace("!",",",str,n_replace);

    EXPECT_EQ(0, count(str.begin(),str.end(),'!'));
    EXPECT_EQ(str.size()/2, count(str.begin(),str.end(),','));
    EXPECT_EQ(str.size(),num * 2);
}
// }}}

// {{{ explode
TEST(StringTest, StringExplodeTest) {

    std::default_random_engine random_engine;
    std::uniform_int_distribution<size_t> num_range(10, 20);

    size_t num = num_range(random_engine);
    std::string str;

    for(size_t i=0 ;i<num; i++)
    {
        str.push_back(static_cast<char>('a'+rand()%26));
        str.push_back('!');
        str.push_back(static_cast<char>('a'+rand()%26));
    }

    std::vector<std::string> v;
    auto str_cols = adbase::explode(str,'!',false);
    EXPECT_EQ(str_cols.size(),num+1);
}
// }}}
