cmake_minimum_required(VERSION 2.6)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
LINK_DIRECTORIES(/usr/local/lib)
if (M32)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -W -static-libstdc++ -m32")  
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -W -gstabs+ -static-libstdc++")  
endif()
SET(LINK_FLAGS "${LINK_FLAGS} -static-libstdc++")

configure_file (
	"${PROJECT_SOURCE_DIR}/adbase/Version.hpp.in"	
	"${PROJECT_SOURCE_DIR}/adbase/Version.hpp"	
)

# libadbase.so 
SET(LIBADBASE_SRC
	Utility/Sha1.cpp
	Utility/String.cpp
	Utility/Serialize.cpp
	Utility/Timestamp.cpp
	Utility/Date.cpp
	Utility/TimeZone.cpp
	Utility/FileSystem.cpp
	Utility/ProcessInfo.cpp
	Utility/Sequence.cpp
	Logging/LogStream.cpp
	Logging/Logging.cpp
	Logging/LogFile.cpp
	Logging/AsyncLogging.cpp
	Config/IniConfig.cpp
	Config/IniParse.cpp
	Net/SocketsOps.cpp
	Net/InetAddress.cpp
	Net/Socket.cpp
	Net/Acceptor.cpp
	Net/Connector.cpp
	Net/Timer.cpp
	Net/TcpConnection.cpp
	Net/TcpClient.cpp
	Net/TcpServer.cpp
	Net/TcpWorker.cpp
	Net/EventLoop.cpp
	Mc/Context.cpp
	Mc/Interface.cpp
	Mc/Ascii.cpp
	Mc/Binary.cpp
	Mc/Handler.cpp
	Mc/ClientHandler.cpp
	Head/Interface.cpp
	Head/Binary.cpp
	Head/Handler.cpp
	Http/Config.cpp
	Http/Request.cpp
	Http/Response.cpp
	Http/Server.cpp
	Metrics/Metrics.cpp
	Metrics/Gauges.cpp
	Metrics/Counter.cpp
	Metrics/Meters.cpp
	Metrics/Histograms.cpp
	Metrics/Timers.cpp
	Metrics/Timer.cpp
)
ADD_LIBRARY(adbase SHARED ${LIBADBASE_SRC})
ADD_LIBRARY(adbase_static STATIC ${LIBADBASE_SRC})
SET_TARGET_PROPERTIES(adbase PROPERTIES VERSION ${ADINFVERSION} SOVERSION ${ADINFSOVERSION})
SET_TARGET_PROPERTIES(adbase_static PROPERTIES OUTPUT_NAME "adbase")
TARGET_LINK_LIBRARIES(adbase event pthread)
TARGET_LINK_LIBRARIES(adbase_static event pthread)
INSTALL(TARGETS adbase adbase_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

if (USEKAFKA)
    # libadbase_kafka.so 
    SET(LIBADBASE_KAFKA_SRC
        Kafka/Producer.cpp
        Kafka/Consumer.cpp
    )
    ADD_LIBRARY(adbase_kafka SHARED ${LIBADBASE_KAFKA_SRC})
    ADD_LIBRARY(adbase_kafka_static STATIC ${LIBADBASE_KAFKA_SRC})
    SET_TARGET_PROPERTIES(adbase_kafka PROPERTIES VERSION ${ADINFVERSION} SOVERSION ${ADINFSOVERSION})
    SET_TARGET_PROPERTIES(adbase_kafka_static PROPERTIES OUTPUT_NAME "adbase_kafka")
    TARGET_LINK_LIBRARIES(adbase_kafka adbase event pthread)
    TARGET_LINK_LIBRARIES(adbase_kafka_static adbase librdkafka.a pthread)
    INSTALL(TARGETS adbase_kafka adbase_kafka_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
endif()

# 安装头文件
INSTALL(DIRECTORY . DESTINATION include/adbase PATTERN ".git" EXCLUDE
	PATTERN "*.cpp" EXCLUDE
	PATTERN "*.swp" EXCLUDE
	PATTERN "*.txt" EXCLUDE
)
