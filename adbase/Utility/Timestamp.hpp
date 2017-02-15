#if !defined ADBASE_UTILITY_HPP_  
# error "Not allow include this header."
#endif

#ifndef ADBASE_UTILITY_TIMESTAMP_HPP_
#define ADBASE_UTILITY_TIMESTAMP_HPP_

#include <string>

namespace adbase {

/**
 * @addtogroup utility adbase::Utility
 */
/*@{*/

/** 
 *  @brief 时间戳工具类
 */
class Timestamp {
public:
	Timestamp() :
		_microSecondsSinceEpoch(0) {
	}	

	explicit Timestamp(int64_t microSecondsSinceEpochArg) :
		_microSecondsSinceEpoch(microSecondsSinceEpochArg) {
	}

	/// 交换 Timestamp 值
	/**
	 *
	 * @param that 交换 Timestamp 值
	 *
	 * @returns void
	 *
	 * @par Example
	 * @code 
     * adbase::Timestamp t1 = adbase::Timestamp::now();
     * adbase::Timestamp t2 = adbase::addTime(t1, 300);
     * std::cout << "t1:" << t1.toFormattedString() << std::endl;
     * std::cout << "t2:" << t2.toFormattedString() << std::endl;
     * t1.swap(t2);
     * std::cout << "t1:" << t1.toFormattedString() << std::endl;
     * std::cout << "t2:" << t2.toFormattedString() << std::endl;
	 * // output
     * t1:20160603 21:54:47.302422
     * t2:20160603 21:59:47.302422
     * t1:20160603 21:59:47.302422
     * t2:20160603 21:54:47.302422
	 * @endcode
	 */
	void swap(Timestamp& that) {
		std::swap(_microSecondsSinceEpoch, that._microSecondsSinceEpoch);	
	}

	/// 将 Timestamp 转化为字符串
	/**
	 *
	 * @returns 转化为字符串
	 *
	 * @par Example
	 * @code 
	 * adbase::Timestamp time(10000010);
	 * std::cout << time.toString() << std::endl;
	 * // output 10.000010
	 * @endcode
	 */
	std::string toString() const;

	/// 将 Timestamp 格式化为可视化字符串
	/**
	 *
	 * @param showMicroseconds 是否显示微妙
	 * @returns 转化为字符串
	 *
	 * @par Example
	 * @code 
	 * adbase::Timestamp time(10000010);
	 * std::cout << time.toFormattedString() << std::endl;
	 * // output 19700101 00:00:10.000010
	 * @endcode
	 */
	std::string toFormattedString(bool showMicroseconds = true) const;

	/// 判断 Timestamp 是否合法
	/**
	 *
	 * @returns 判断是否合法，合法：true
	 *
	 * @par Example
	 * @code 
	 * adbase::Timestamp invlidTime(-1);
	 * if (invlidTime.valid()) {
	 * 	std::cout << "Timestamp valid" << std::endl;
	 * } else {
	 * 	std::cout << "Timestamp invalid" << std::endl;
	 * }
	 * @endcode
	 */
	bool valid() const {
		return _microSecondsSinceEpoch > 0;	
	}

	/// 获取 Timestamp 微秒数
	/**
	 *
	 * @returns  微秒数
	 *
	 */
	int64_t microSecondsSinceEpoch() const {
		return _microSecondsSinceEpoch;	
	}

	/// 获取 Timestamp 秒数
	/**
	 *
	 * @returns  秒数
	 *
	 */
	time_t secondsSinceEpoch() const {
		return static_cast<time_t>(_microSecondsSinceEpoch / kMicroSecondsPerSecond);	
	}

	/// 返回当前时间的 Timestamp 对象
	/**
	 *
	 * @returns  Timestamp 对象
	 *
	 */
	static Timestamp now();

	static Timestamp invalid() {
		return Timestamp();	
	}

	/// 通过 unix time 构建 Timestamp 对象
	/**
	 *
	 * @returns  Timestamp 对象
	 *
	 */
	static Timestamp fromUnixTime(time_t t) {
		return fromUnixTime(t, 0);	
	}

	/// 通过 unix time 构建 Timestamp 对象, 并且设置微秒数
	/**
	 *
	 * @returns  Timestamp 对象
	 *
	 */
	static Timestamp fromUnixTime(time_t t, int microseconds) {
		return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);	
	}

	static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
	int64_t _microSecondsSinceEpoch;
};	


/// 比较两个 Timestamp 大小
/**
 *
 * @returns  bool
 *
 */
inline bool operator<(Timestamp lhs, Timestamp rhs) {
	return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();	
}

/// 判断两个 Timestamp 是否相等
/**
 *
 * @returns  bool
 *
 */
inline bool operator==(Timestamp lhs, Timestamp rhs) {
	return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();	
}

/// 求两个 Timestamp 的差值
/**
 *
 * @returns  bool
 *
 */
inline double timeDifference(Timestamp high, Timestamp low) {
	int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();	
	return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
	int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microSecondsSinceEpoch() + delta);	
}

/*@}*/
}

#endif
