INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(../adbase/)
LINK_DIRECTORIES(/usr/local/lib)
if (M32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -W -static-libstdc++ -m32")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -W -gstabs+ -static-libstdc++")
endif()
SET(LINK_FLAGS "${LINK_FLAGS} -static-libstdc++")

SET(TEST_ALL
	TestAll.cpp
	LogStreamTest.cpp
	Buffer.cpp
	Date.cpp
	Enbian.cpp
	Queue.cpp
    Sequence.cpp
    String.cpp
)
ADD_EXECUTABLE(test_all ${TEST_ALL})
TARGET_LINK_LIBRARIES(test_all adbase libgtest.a libgtest_main.a pthread)
