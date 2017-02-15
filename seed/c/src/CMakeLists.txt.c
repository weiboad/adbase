INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(./thridpart)
LINK_DIRECTORIES(/usr/local/lib)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -W -static-libstdc++")  

configure_file (
	"${PROJECT_SOURCE_DIR}/src/Version.hpp.in"	
	"${PROJECT_SOURCE_DIR}/src/Version.hpp"	
)

configure_file (
	"${PROJECT_SOURCE_DIR}/rpm/@ADINF_PROJECT_NAME@.spec.in"	
	"${PROJECT_SOURCE_DIR}/rpm/@ADINF_PROJECT_NAME@.spec"	
)

configure_file (
	"${PROJECT_SOURCE_DIR}/rpm/build_rpm.in"	
	"${PROJECT_SOURCE_DIR}/rpm/build_rpm"	
	@ONLY
)

SET(@ADINF_PROJECT_NAME|upper@_SRC @ADINF_PROJECT_NAME@.cpp
	BootStrap.cpp	
	App.cpp	
	//@IF @adserver
	AdServer.cpp
	Http.cpp
	Http/HttpInterface.cpp
	Http/Server.cpp
	//@FOR @http_controllers
	Http/@REPLACE0@.cpp
	//@ENDFOR
	McProcessor.cpp
	HeadProcessor.cpp
	//@ENDIF
	//@IF @kafkac || @kafkap
	Aims.cpp
	//@ENDIF
	//@IF @kafkac
	//@FOR @kafka_consumers
	Aims/Kafka/Consumer@REPLACE0@.cpp
	//@ENDFOR
	//@ENDIF
	//@IF @kafkap
	//@FOR @kafka_producers
	Aims/Kafka/Producer@REPLACE0@.cpp
	//@ENDFOR
	//@ENDIF
	Timer.cpp
)
ADD_EXECUTABLE(@ADINF_PROJECT_NAME@ ${@ADINF_PROJECT_NAME|upper@_SRC})
TARGET_LINK_LIBRARIES(@ADINF_PROJECT_NAME@ libadbase.a 
	//@IF @kafkac || @kafkap
	libadbase_kafka.a
	librdkafka.a 
	//@ENDIF
	libevent.a
	pthread
	rt
	//@IF @kafkac || @kafkap
	z
	//@ENDIF
	)
INSTALL(TARGETS @ADINF_PROJECT_NAME@ RUNTIME DESTINATION adinf/@ADINF_PROJECT_NAME@/bin)
