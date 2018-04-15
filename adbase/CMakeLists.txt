cmake_minimum_required(VERSION 2.6)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

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
SET_TARGET_PROPERTIES(adbase PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} SOVERSION ${PROJECT_VERSION_MAJOR})
SET_TARGET_PROPERTIES(adbase_static PROPERTIES OUTPUT_NAME "adbase")
INSTALL(TARGETS adbase adbase_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
# 安装头文件
INSTALL(DIRECTORY . DESTINATION include/adbase PATTERN ".git" EXCLUDE
	PATTERN "*.cpp" EXCLUDE
	PATTERN "*.swp" EXCLUDE
	PATTERN "*.txt" EXCLUDE
)

SET(CMAKE_SOURCE_DIR .)
SET(CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules ${CMAKE_SOURCE_DIR}/cmake/Modules
		${PROJECT_SOURCE_DIR}/Modules)

# event
FIND_PACKAGE( libevent REQUIRED)
MARK_AS_ADVANCED(
	LIBEVENT_INCLUDE_DIR
	LIBEVENT_LIBRARIES
)
IF (LIBEVENT_INCLUDE_DIR AND LIBEVENT_LIBRARIES)
	MESSAGE(STATUS "Found libevent libraries")
	INCLUDE_DIRECTORIES(${LIBEVENT_INCLUDE_DIR})
	MESSAGE( ${LIBEVENT_LIBRARIES} )
	MESSAGE( ${LIBEVENT_STATIC_LIBRARIES} )
	TARGET_LINK_LIBRARIES(adbase ${LIBEVENT_LIBRARIES} )
ELSE (LIBEVENT_INCLUDE_DIR AND LIBEVENT_LIBRARIES)
	MESSAGE(FATAL_ERROR "Failed to find libevent libraries")
ENDIF (LIBEVENT_INCLUDE_DIR AND LIBEVENT_LIBRARIES)

# pthread
FIND_PACKAGE( libpthread REQUIRED)
MARK_AS_ADVANCED(
LIBPTHREAD_INCLUDE_DIR
LIBPTHREAD_LIBRARIES
)
IF (LIBPTHREAD_INCLUDE_DIR AND LIBPTHREAD_LIBRARIES)
    MESSAGE(STATUS "Found libpthread libraries")
    INCLUDE_DIRECTORIES(${LIBPTHREAD_INCLUDE_DIR})
    MESSAGE( ${LIBPTHREAD_LIBRARIES} )
    TARGET_LINK_LIBRARIES(adbase ${LIBPTHREAD_LIBRARIES} )
    TARGET_LINK_LIBRARIES(adbase_static ${LIBPTHREAD_LIBRARIES} )
ELSE (LIBPTHREAD_INCLUDE_DIR AND LIBPTHREAD_LIBRARIES)
    MESSAGE(FATAL_ERROR "Failed to find libpthread libraries")
ENDIF (LIBPTHREAD_INCLUDE_DIR AND LIBPTHREAD_LIBRARIES)

IF (INSTALL_KAFKA)
    # libadbase_kafka.so 
    SET(LIBADBASE_KAFKA_SRC
        Kafka/Producer.cpp
        Kafka/Consumer.cpp
    )
    ADD_LIBRARY(adbase_kafka SHARED ${LIBADBASE_KAFKA_SRC})
    ADD_LIBRARY(adbase_kafka_static STATIC ${LIBADBASE_KAFKA_SRC})
    SET_TARGET_PROPERTIES(adbase_kafka PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} SOVERSION ${PROJECT_VERSION_MAJOR})
    SET_TARGET_PROPERTIES(adbase_kafka_static PROPERTIES OUTPUT_NAME "adbase_kafka")
    TARGET_LINK_LIBRARIES(adbase_kafka adbase)
    INSTALL(TARGETS adbase_kafka adbase_kafka_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
    FIND_PACKAGE( librdkafka REQUIRED)
    MARK_AS_ADVANCED(
    LIBRDKAFKA_INCLUDE_DIR
    LIBRDKAFKA_LIBRARIES
    )
    IF (LIBRDKAFKA_INCLUDE_DIR AND LIBRDKAFKA_LIBRARIES)
        MESSAGE(STATUS "Found librdkafka libraries")
        INCLUDE_DIRECTORIES(${LIBRDKAFKA_INCLUDE_DIR})
        MESSAGE( ${LIBRDKAFKA_LIBRARIES} )
        TARGET_LINK_LIBRARIES(adbase_kafka ${LIBRDKAFKA_LIBRARIES} )
    ELSE (LIBRDKAFKA_INCLUDE_DIR AND LIBRDKAFKA_LIBRARIES)
        MESSAGE(FATAL_ERROR "Failed to find librdkafka libraries")
    ENDIF (LIBRDKAFKA_INCLUDE_DIR AND LIBRDKAFKA_LIBRARIES)
ENDIF (INSTALL_KAFKA)

IF (INSTALL_LUA)
    # libadbase_lua.so 
    SET(LIBADBASE_LUA_SRC
	Lua/BindingManager.cpp
	Lua/BindingNamespace.cpp
	Lua/BindingUtils.cpp
	Lua/Engine.cpp
    )
    ADD_LIBRARY(adbase_lua SHARED ${LIBADBASE_LUA_SRC})
    ADD_LIBRARY(adbase_lua_static STATIC ${LIBADBASE_LUA_SRC})
    SET_TARGET_PROPERTIES(adbase_lua PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} SOVERSION ${PROJECT_VERSION_MAJOR})
    SET_TARGET_PROPERTIES(adbase_lua_static PROPERTIES OUTPUT_NAME "adbase_lua")
    TARGET_LINK_LIBRARIES(adbase_lua adbase)
    INSTALL(TARGETS adbase_lua adbase_lua_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
    FIND_PACKAGE( liblua REQUIRED)
    MARK_AS_ADVANCED(
    LIBLUA_INCLUDE_DIR
    LIBLUA_LIBRARIES
    )
    IF (LIBLUA_INCLUDE_DIR AND LIBLUA_LIBRARIES)
        MESSAGE(STATUS "Found liblua libraries")
        INCLUDE_DIRECTORIES(${LIBLUA_INCLUDE_DIR})
        MESSAGE( ${LIBLUA_LIBRARIES} )
    ELSE (LIBLUA_INCLUDE_DIR AND LIBLUA_LIBRARIES)
        MESSAGE(FATAL_ERROR "Failed to find liblua libraries")
    ENDIF (LIBLUA_INCLUDE_DIR AND LIBLUA_LIBRARIES)
ENDIF (INSTALL_LUA)
