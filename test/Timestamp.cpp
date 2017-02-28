#include <adbase/Utility.hpp>
#include <gtest/gtest.h>
#include <random>
#include <thread>

// {{{
TEST(TimerstampTest, TimerstampAddTest) {
    adbase::Timestamp t1 = adbase::Timestamp::now();

    size_t count = 10;
    for (size_t i = 0; i < count; i++)
    {
        srand(static_cast<unsigned>(time(NULL)) );
        int n_addation = rand() % 30 + rand() % 3;
        adbase::Timestamp t2 = adbase::addTime(t1,n_addation);
        double time_diff = adbase::timeDifference(t2,t1);
        EXPECT_EQ(time_diff,n_addation);
    }
}
// }}}

// {{{
TEST(TimerstampTest, TimerstampSwapTest) {
    adbase::Timestamp t1 = adbase::Timestamp::now();
    adbase::Timestamp t2 = adbase::addTime(t1,300);

    t1.swap(t2);
    double time_diff = adbase::timeDifference(t2,t1);
    EXPECT_EQ(time_diff,-300);
}
// }}}

// {{{
TEST(TimerstampTest, TimerstampCmpTest) {
    adbase::Timestamp t1 = adbase::Timestamp::now();

    //default assignment operator.
    adbase::Timestamp t2 = t1;
    EXPECT_EQ(t1 == t2,true);

    adbase::Timestamp t3 = adbase::addTime(t1,1);
    EXPECT_EQ(t1 < t3,true);
    EXPECT_EQ(t2 < t3,true);
}
// }}}

// {{{
TEST(TimerstampTest, TimerstampToStringTest) {
    adbase::Timestamp t1 = adbase::Timestamp::now();
    std::string str_format = t1.toFormattedString();
    std::string str = t1.toString();
    EXPECT_EQ(std::string(typeid(str).name()),std::string("Ss"));
    EXPECT_EQ(std::string(typeid(str_format).name()),std::string("Ss"));
    EXPECT_EQ(str.size() > 0 && str.size() <= 32,true);
    EXPECT_EQ(str.size() > 0 && str_format.size() <= 32,true);
}
// }}}

// {{{
TEST(TimerstampTest, TimerstampFromUnixTimeTest) {
    adbase::Timestamp t1 = adbase::Timestamp::now();
    time_t t1_since_epoch = t1.secondsSinceEpoch();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    adbase::Timestamp t2 = adbase::Timestamp::now();
    time_t t2_since_epoch = t2.secondsSinceEpoch();

    EXPECT_EQ((t2_since_epoch - t1_since_epoch) == 1,true);
}
// }}}
