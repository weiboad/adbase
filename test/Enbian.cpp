#include <adbase/Utility.hpp>
#include <gtest/gtest.h>
#include <cassert>

template<typename T>
std::string convertString(T value) {
	char bytes[sizeof(T)];
	memcpy(bytes, &value, sizeof(T));
	return adbase::bytesToHexString(bytes, sizeof(T));
}

TEST(EnbianTest, EnbianHostToNet) {
	uint64_t test = 1234;
	# if __BYTE_ORDER == __LITTLE_ENDIAN
	EXPECT_EQ(convertString<uint64_t>(test), "d204000000000000");	
	# else 
	EXPECT_EQ(convertString<uint64_t>(test), "00000000000004d2");	
	# endif
	test = adbase::hostToNetwork64(test);
	EXPECT_EQ(convertString<uint64_t>(test), "00000000000004d2");	

	uint32_t test32 = 1234;
	# if __BYTE_ORDER == __LITTLE_ENDIAN
	EXPECT_EQ(convertString<uint32_t>(test32), "d2040000");	
	# else 
	EXPECT_EQ(convertString<uint32_t>(test32), "000004d2");	
	# endif
	test32 = adbase::hostToNetwork32(test32);
	EXPECT_EQ(convertString<uint32_t>(test32), "000004d2");	

	uint16_t test16 = 1234;
	# if __BYTE_ORDER == __LITTLE_ENDIAN
	EXPECT_EQ(convertString<uint16_t>(test16), "d204");	
	# else 
	EXPECT_EQ(convertString<uint16_t>(test16), "04d2");	
	# endif
	test16 = adbase::hostToNetwork16(test16);
	EXPECT_EQ(convertString<uint16_t>(test16), "04d2");	
}

TEST(EnbianTest, EnbianNetToHost) {
	uint64_t test = 0xd204000000000000;
	test = adbase::networkToHost64(test);
	# if __BYTE_ORDER == __LITTLE_ENDIAN
		EXPECT_EQ(convertString<uint64_t>(test), "d204000000000000");	
	# else 
		EXPECT_EQ(convertString<uint64_t>(test), "d204000000000000");	
	# endif

	uint32_t test32 = 0xd2040000;
	test32 = adbase::networkToHost32(test32);
	# if __BYTE_ORDER == __LITTLE_ENDIAN
		EXPECT_EQ(convertString<uint32_t>(test32), "d2040000");	
	# else 
		EXPECT_EQ(convertString<uint32_t>(test32), "d2040000");	
	# endif

	uint16_t test16 = 0xd204;
	test16 = adbase::networkToHost16(test16);
	# if __BYTE_ORDER == __LITTLE_ENDIAN
		EXPECT_EQ(convertString<uint16_t>(test16), "d204");	
	# else 
		EXPECT_EQ(convertString<uint16_t>(test16), "d204");	
	# endif
}
