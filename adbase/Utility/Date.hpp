#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_DATE_HPP_
#define ADBASE_UTILITY_DATE_HPP_

#include <iostream>

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/

/** 
 *  @brief 日期工具类
 */
class Date {
public:
	struct YearMonthDay {
		int year;
		int month;
		int day;	
	};	

	static const int kDaysPerWeek = 7;
	static const int kJulianDayOf1970_01_01;

	/// Constucts an invalid Date.
	Date() : _julianDayNumber(0) {}

	/// Constucts a yyyy-mm-dd Date. 
	Date(int year, int month, int day);

	/// Constucts a Date from Julian Day Number. 
	explicit Date(int julianDayNum) : _julianDayNumber(julianDayNum) {} 

	/// Constucts a Date from struct tm
	explicit Date(const struct tm&);

	/// default copy/assignment/dtor are Okay
	void swap(Date& that) {
		::std::swap(_julianDayNumber, that._julianDayNumber);	
	}

	/// Date is valid
	bool valid() const {
		return _julianDayNumber > 0;	
	}

	::std::string toIsoString() const;

	struct YearMonthDay yearMonthDay() const;

	int year() const {
		return yearMonthDay().year;		
	}

	int month() const {
		return yearMonthDay().month;	
	}

	int day() const {
		return yearMonthDay().day;	
	}

	/// [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
	int weekDay() const {
		return 	(_julianDayNumber + 1) % kDaysPerWeek;
	}

	int julianDayNumber() const {
		return _julianDayNumber;	
	}

	friend inline bool operator<(Date x, Date y);
	friend inline bool operator>(Date x, Date y);
	friend inline bool operator==(Date x, Date y);

private:
	int _julianDayNumber;
};

inline bool operator<(Date x, Date y) {
	return x._julianDayNumber < y._julianDayNumber;	
}

inline bool operator>(Date x, Date y) {
	return x._julianDayNumber > y._julianDayNumber;	
}

inline bool operator==(Date x, Date y) {
	return x._julianDayNumber == y._julianDayNumber;	
}

/*@}*/
}
#endif
