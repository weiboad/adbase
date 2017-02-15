#include <adbase/Utility.hpp>
#include <gtest/gtest.h>

TEST(DateTest, DateConstruct) {
	adbase::Date date;
	EXPECT_FALSE(date.valid());
	adbase::Date date1(1970, 1, 1);
	EXPECT_EQ(date1.toIsoString(), "1970-01-01");
	adbase::Date date2(2457545);
	EXPECT_EQ(date2.toIsoString(), "2016-06-05");

	date1.swap(date2);
	EXPECT_EQ(date1.toIsoString(), "2016-06-05");
	EXPECT_EQ(date2.toIsoString(), "1970-01-01");
	EXPECT_TRUE(date1.valid());
	EXPECT_TRUE(date2.valid());

	EXPECT_EQ(adbase::Date::kJulianDayOf1970_01_01, 2440588);
}

TEST(DateTest, DateJulianDayNumber) {
	adbase::Date date(2016, 6, 5);
	EXPECT_EQ(date.julianDayNumber(), 2457545);
}

TEST(DateTest, DateDetail) {
	adbase::Date date(2016, 6, 5);
	EXPECT_EQ(date.weekDay(), 0);
	EXPECT_EQ(date.day(), 5);
	EXPECT_EQ(date.month(), 6);
	EXPECT_EQ(date.year(), 2016);

	adbase::Date date1(2016, 6, 5);
	adbase::Date date2(2016, 8, 4);
	EXPECT_FALSE(date1 > date2);
	EXPECT_TRUE(date1 < date2);
	EXPECT_TRUE(date1 == date);
}
