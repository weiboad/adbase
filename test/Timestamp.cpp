#include <adbase/Utility.hpp>
#include <gtest/gtest.h>
#include <random>

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
