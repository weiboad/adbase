MESSAGE(STATUS "Using bundled Findliblua.cmake...")
  FIND_PATH(
  LIBLUA_INCLUDE_DIR
  lua.h
  /usr/include/
  /usr/local/include/
  )

FIND_LIBRARY(
  LIBLUA_LIBRARIES NAMES lua liblua.a
  PATHS /usr/lib/ /usr/local/lib/ /usr/lib64 /usr/local/lib64
  )
