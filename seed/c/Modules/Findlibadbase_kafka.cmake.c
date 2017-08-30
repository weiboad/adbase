MESSAGE(STATUS "Using bundled Findlibadbase_kafka.cmake...")
  FIND_PATH(
  LIBADBASE_KAFKA_INCLUDE_DIR
  Kafka.hpp
  /usr/local/include/adbase
  /usr/include/
  )

FIND_LIBRARY(
  LIBADBASE_KAFKA_LIBRARIES NAMES libadbase_kafka.a
  PATHS /usr/lib/ /usr/local/lib/ /usr/lib64 /usr/local/lib64
  )
