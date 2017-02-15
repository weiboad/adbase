#include <adbase/Utility.hpp>
#include <gtest/gtest.h>

// {{{ construct

TEST(BufferTest, testBufferConstruct) {
	adbase::Buffer buffer;	
	EXPECT_EQ(buffer.readableBytes(), 0);
	EXPECT_EQ(buffer.writableBytes(), 1024);
	EXPECT_EQ(buffer.prependableBytes(), 0);
}

// }}}
// {{{ append

TEST(BufferTest, testBufferAppend) {
	adbase::Buffer buffer;
	buffer.appendInt8(std::numeric_limits<int8_t>::min());
	EXPECT_EQ(buffer.readableBytes(), sizeof(int8_t));
	EXPECT_EQ(buffer.readInt8(), -128);

	buffer.appendInt8(std::numeric_limits<int8_t>::max());
	EXPECT_EQ(buffer.readInt8(), 127);

	buffer.appendInt16(std::numeric_limits<int16_t>::min());
	EXPECT_EQ(buffer.readableBytes(), sizeof(int16_t));
	EXPECT_EQ(buffer.readInt16(), -32768);

	buffer.appendInt16(std::numeric_limits<int16_t>::max());
	EXPECT_EQ(buffer.readInt16(), 32767);

	buffer.appendInt32(std::numeric_limits<int32_t>::min());
	EXPECT_EQ(buffer.readableBytes(), sizeof(int32_t));
	EXPECT_EQ(buffer.readInt32(), -2147483648);

	buffer.appendInt32(std::numeric_limits<int32_t>::max());
	EXPECT_EQ(buffer.readInt32(), 2147483647);

	buffer.appendInt64(std::numeric_limits<int64_t>::min());
	EXPECT_EQ(buffer.readableBytes(), sizeof(int64_t));
	EXPECT_EQ(std::to_string(buffer.readInt64()), "-9223372036854775808");

	buffer.appendInt64(std::numeric_limits<int64_t>::max());
	EXPECT_EQ(buffer.readInt64(), 9223372036854775807);

	std::string testStr = "hello";
	buffer.append(testStr);
	EXPECT_EQ(buffer.readableBytes(), 5);
	EXPECT_EQ(buffer.retrieveAllAsString(), testStr);

	buffer.append(testStr.c_str(), testStr.size());
	EXPECT_EQ(buffer.readableBytes(), 5);
	EXPECT_EQ(buffer.retrieveAllAsString(), testStr);

	buffer.append(testStr.c_str(), testStr.size());
	EXPECT_EQ(buffer.readableBytes(), 5);
	EXPECT_EQ(buffer.retrieveAsString(2), "he");
	EXPECT_EQ(buffer.readableBytes(), 3);
}
// }}}
// {{{ internalCapacity

TEST(BufferTest, testBufferInternalCapacity) {
	adbase::Buffer buffer;
	EXPECT_EQ(buffer.internalCapacity(), 1024);
	for (uint64_t i = 0; i < 10000; i++) {
		buffer.appendInt64(i);
	}
	EXPECT_GT(buffer.internalCapacity(), sizeof(uint64_t) * 10000);
	buffer.shrink();
	EXPECT_EQ(buffer.internalCapacity(), sizeof(uint64_t) * 10000);
}
// }}}
// {{{ prepend

TEST(BufferTest, testBufferPrepend) {
	adbase::Buffer buffer;
	buffer.appendInt8(std::numeric_limits<int8_t>::min());
	buffer.appendInt8(std::numeric_limits<int8_t>::min());
	EXPECT_EQ(buffer.readableBytes(), 2 * sizeof(int8_t));
	EXPECT_EQ(buffer.readInt8(), -128);
	EXPECT_EQ(buffer.prependableBytes(), sizeof(int8_t));
	buffer.prependInt8(std::numeric_limits<int8_t>::min());
	EXPECT_EQ(buffer.readableBytes(), 2 * sizeof(int8_t));
	buffer.retrieveAll();

	buffer.appendInt16(std::numeric_limits<int16_t>::min());
	buffer.appendInt16(std::numeric_limits<int16_t>::min());
	EXPECT_EQ(buffer.readableBytes(), 2 * sizeof(int16_t));
	EXPECT_EQ(buffer.readInt16(), -32768);
	EXPECT_EQ(buffer.prependableBytes(), sizeof(int16_t));
	buffer.prependInt16(std::numeric_limits<int16_t>::min());
	EXPECT_EQ(buffer.readableBytes(), 2 * sizeof(int16_t));
	buffer.retrieveAll();

	buffer.appendInt32(std::numeric_limits<int32_t>::min());
	buffer.appendInt32(std::numeric_limits<int32_t>::min());
	EXPECT_EQ(buffer.readableBytes(), 2 * sizeof(int32_t));
	EXPECT_EQ(buffer.readInt32(), -2147483648);
	EXPECT_EQ(buffer.prependableBytes(), sizeof(int32_t));
	buffer.prependInt32(std::numeric_limits<int32_t>::min());
	EXPECT_EQ(buffer.readableBytes(), 2 * sizeof(int32_t));
	buffer.retrieveAll();

	buffer.appendInt64(std::numeric_limits<int64_t>::min());
	buffer.appendInt64(std::numeric_limits<int64_t>::min());
	EXPECT_EQ(buffer.readableBytes(), 2 * sizeof(int64_t));
	EXPECT_EQ(std::to_string(buffer.readInt64()), "-9223372036854775808");
	EXPECT_EQ(buffer.prependableBytes(), sizeof(int64_t));
	buffer.prependInt64(std::numeric_limits<int64_t>::min());
	EXPECT_EQ(buffer.readableBytes(), 2 * sizeof(int64_t));
	buffer.retrieveAll();

	std::string testStr = "hello";
	buffer.append(testStr);
	EXPECT_EQ(buffer.readableBytes(), 5);
	EXPECT_EQ(buffer.retrieveAsString(2), "he");
	EXPECT_EQ(buffer.prependableBytes(), 2);
	std::string prependStr = "ge";
	buffer.prepend(prependStr.c_str(), prependStr.size());
	EXPECT_EQ(buffer.readableBytes(), 5);
	EXPECT_EQ(buffer.retrieveAllAsString(), "gello");
}
// }}}
