MESSAGE(STATUS "Using bundled Findlibevent.cmake...")
  FIND_PATH(
  LIBEVENT_INCLUDE_DIR
  event.h
  /usr/include/
  /usr/local/include/event2
  )

FIND_LIBRARY(
  LIBEVENT_LIBRARIES NAMES event
  PATHS /usr/lib/ /usr/local/lib/ /usr/lib64 /usr/local/lib64
  )
FIND_LIBRARY(
  LIBEVENT_STATIC_LIBRARIES NAMES libevent.a
  PATHS /usr/lib/ /usr/local/lib/ /usr/lib64 /usr/local/lib64
  )
