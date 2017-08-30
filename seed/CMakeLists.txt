INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../adbase/)
LINK_DIRECTORIES(/usr/local/lib)
if (M32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -W -static-libstdc++ -m32")  
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -W -gstabs+ -static-libstdc++")  
endif()
SET(LINK_FLAGS "${LINK_FLAGS} -static-libstdc++")

SET(SEED_UTIL Seed.cpp)
ADD_EXECUTABLE(adbase_seed ${SEED_UTIL})
TARGET_LINK_LIBRARIES(adbase_seed adbase)
INSTALL(TARGETS adbase_seed RUNTIME DESTINATION bin)
INSTALL(PROGRAMS adbase_skeleton DESTINATION bin)
INSTALL(DIRECTORY c/ DESTINATION /var/lib/adbase PATTERN ".git" EXCLUDE
	PATTERN "*.swp" EXCLUDE
)
