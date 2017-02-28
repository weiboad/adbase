#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>
#include <time.h>

void printDate(const adbase::Date& date) {
    // get year month day
    LOG_INFO << "Year: " << date.year();
    LOG_INFO << "Month: " << date.month();
    LOG_INFO << "Day: " << date.day();
    // get week day
    LOG_INFO << "WeekDay:" << date.weekDay();
    // get julian day
    LOG_INFO << "julianDayNumber:" << date.julianDayNumber();
}

int main(void) {
    // use year month day contruct
    LOG_INFO << "Use year/month/day contruct";
    adbase::Date date1(2017, 2, 27);
    printDate(date1);

    LOG_INFO << "Use julian day contruct";
    adbase::Date date2(2457813);
    printDate(date2);

    struct tm* ptr;
    time_t rawtime;
    time(&rawtime);
    ptr = gmtime(&rawtime);
    LOG_INFO << "Use tm day contruct";
    adbase::Date date3(*ptr);
    printDate(date3);

    LOG_INFO << "Date > opt: " << (date2 > date1);
    return 0;
}
