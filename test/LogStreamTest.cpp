#include <adbase/Utility.hpp>
#include <adbase/Logging.hpp>

#include <gtest/gtest.h>

TEST(LogStreamTest, testLogStreamBoolean) {
	adbase::LogStream os;
	const adbase::LogStream::Buffer& buf = os.buffer();
	EXPECT_EQ(buf.toString(), std::string(""));		
	os << true;
	EXPECT_EQ(os.buffer().toString(), std::string("1"));
	os << '\n';
	EXPECT_EQ(buf.toString(), std::string("1\n"));
	os << false;
	EXPECT_EQ(buf.toString(), std::string("1\n0"));
}

TEST(LogStreamTest, testLogStreamIntegers) {
	adbase::LogStream os;
	const adbase::LogStream::Buffer& buf = os.buffer();
	EXPECT_EQ(buf.toString(), std::string(""));
	os << 1;
	EXPECT_EQ(buf.toString(), std::string("1"));
	os << 0;
	EXPECT_EQ(buf.toString(), std::string("10"));
	os << -1;
	EXPECT_EQ(buf.toString(), std::string("10-1"));
	os.resetBuffer();

	os << 0 << " " << 123 << 'x' << 0x64;
	EXPECT_EQ(buf.toString(), std::string("0 123x100"));
}

TEST(LogStreamTest, testLogStreamIntegerLimits) {
	adbase::LogStream os;
	const adbase::LogStream::Buffer& buf = os.buffer();
	os << -2147483647;
	EXPECT_EQ(buf.toString(), std::string("-2147483647"));
	os << static_cast<int>(-2147483647 - 1);
	EXPECT_EQ(buf.toString(), std::string("-2147483647-2147483648"));
	os.resetBuffer();

	os << std::numeric_limits<int16_t>::min();
	EXPECT_EQ(buf.toString(), std::string("-32768"));
	os.resetBuffer();

	os << std::numeric_limits<int16_t>::max();
	EXPECT_EQ(buf.toString(), std::string("32767"));
	os.resetBuffer();

	os << std::numeric_limits<uint16_t>::min();
	EXPECT_EQ(buf.toString(), std::string("0"));
	os.resetBuffer();

	os << std::numeric_limits<uint16_t>::max();
	EXPECT_EQ(buf.toString(), std::string("65535"));
	os.resetBuffer();

	os << std::numeric_limits<int32_t>::min();
	EXPECT_EQ(buf.toString(), std::string("-2147483648"));
	os.resetBuffer();

	os << std::numeric_limits<int32_t>::max();
	EXPECT_EQ(buf.toString(), std::string("2147483647"));
	os.resetBuffer();

	os << std::numeric_limits<uint32_t>::min();
	EXPECT_EQ(buf.toString(), std::string("0"));
	os.resetBuffer();

	os << std::numeric_limits<uint32_t>::max();
	EXPECT_EQ(buf.toString(), std::string("4294967295"));
	os.resetBuffer();

	os << std::numeric_limits<int64_t>::min();
	EXPECT_EQ(buf.toString(), std::string("-9223372036854775808"));
	os.resetBuffer();

	os << std::numeric_limits<int64_t>::max();
	EXPECT_EQ(buf.toString(), std::string("9223372036854775807"));
	os.resetBuffer();

	os << std::numeric_limits<uint64_t>::min();
	EXPECT_EQ(buf.toString(), std::string("0"));
	os.resetBuffer();

	os << std::numeric_limits<uint64_t>::max();
	EXPECT_EQ(buf.toString(), std::string("18446744073709551615"));
	os.resetBuffer();

	int16_t a = 0;
	int32_t b = 0;
	int64_t c = 0;
	os << a;
	os << b;
	os << c;
	EXPECT_EQ(buf.toString(), std::string("000"));
}

TEST(LogStreamTest, testLogStreamFloats) {
	adbase::LogStream os;
	const adbase::LogStream::Buffer& buf = os.buffer();

	os << 0.0;
	EXPECT_EQ(buf.toString(), std::string("0"));
	os.resetBuffer();
	
	os << 1.0;
	EXPECT_EQ(buf.toString(), std::string("1"));
	os.resetBuffer();

	os << 0.05;
	EXPECT_EQ(buf.toString(), std::string("0.05"));
	os.resetBuffer();

	os << 0.15;
	EXPECT_EQ(buf.toString(), std::string("0.15"));
	os.resetBuffer();

	double a = 0.1;
	os << a;
	EXPECT_EQ(buf.toString(), std::string("0.1"));
	os.resetBuffer();

	double b = 0.05;
	os << b;
	EXPECT_EQ(buf.toString(), std::string("0.05"));
	os.resetBuffer();

	double c = 0.15;
	os << c;
	EXPECT_EQ(buf.toString(), std::string("0.15"));
	os.resetBuffer();

	os << a + b;
	EXPECT_EQ(buf.toString(), std::string("0.15"));
	os.resetBuffer();

	os << 1.23456789;
	EXPECT_EQ(buf.toString(), std::string("1.23456789"));
	os.resetBuffer();

	os << 1.234567;
	EXPECT_EQ(buf.toString(), std::string("1.234567"));
	os.resetBuffer();

	os << -123.4567;
	EXPECT_EQ(buf.toString(), std::string("-123.4567"));
	os.resetBuffer();
}

TEST(LogStreamTest, testLogStreamVoid) {
	adbase::LogStream os;
	const adbase::LogStream::Buffer& buf = os.buffer();

	os << static_cast<void *>(0);
	EXPECT_EQ(buf.toString(), std::string("0x0"));
	os.resetBuffer();

	os << reinterpret_cast<void *>(8888);
	EXPECT_EQ(buf.toString(), std::string("0x22B8"));
	os.resetBuffer();
}

TEST(LogStreamTest, testLogStreamStrings) {
	adbase::LogStream os;
	const adbase::LogStream::Buffer& buf = os.buffer();
	
	os << "Hello ";	
	EXPECT_EQ(buf.toString(), std::string("Hello "));
	std::string adinf = "ADINF";
	os << adinf;
	EXPECT_EQ(buf.toString(), std::string("Hello ADINF"));
}

TEST(LogStreamTest, testLogStreamFmts) {
	adbase::LogStream os;
	const adbase::LogStream::Buffer& buf = os.buffer();
	
	os << adbase::Fmt("%4d", 1);
	EXPECT_EQ(buf.toString(), std::string("   1"));
	os.resetBuffer();
	
	os << adbase::Fmt("%4.2f", 1.2);
	EXPECT_EQ(buf.toString(), std::string("1.20"));
	os.resetBuffer();
	
	os << adbase::Fmt("%4.2f", 1.2) << adbase::Fmt("%4d", 43);
	EXPECT_EQ(buf.toString(), std::string("1.20  43"));
	os.resetBuffer();
}

TEST(LogStreamTest, testLogStreamLong) {
	adbase::LogStream os;
	const adbase::LogStream::Buffer& buf = os.buffer();
	
	for (int i = 0; i < 399; ++i) {
		os << "123456789 ";	
		EXPECT_EQ(buf.length(), 10 * (i + 1));
		EXPECT_EQ(buf.avail(), 4000 - 10 * (i + 1));
	}	

	EXPECT_EQ(buf.length(), 3990);
	EXPECT_EQ(buf.avail(), 10);

	os << "abcdefghi";
	EXPECT_EQ(buf.length(), 3999);
	EXPECT_EQ(buf.avail(), 1);
}
