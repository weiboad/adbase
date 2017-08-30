MESSAGE(STATUS "Using bundled Findlibadbase.cmake...")
  FIND_PATH(
  LIBADBASE_INCLUDE_DIR
  Version.hpp
  /usr/local/include/adbase
  /usr/include/
  )

FIND_LIBRARY(
  LIBADBASE_LIBRARIES NAMES libadbase.a
  PATHS /usr/lib/ /usr/local/lib/ /usr/lib64 /usr/local/lib64
  )
