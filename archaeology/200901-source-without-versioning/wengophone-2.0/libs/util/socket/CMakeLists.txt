project(owsocket)

# needed include directories to build owsocket
# saves the variable in internal cache for later use
set(OWSOCKET_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  ${OWTHREAD_INCLUDE_DIRS}
  CACHE INTERNAL "owsocket include directories"
)

set(OWSOCKET_LIBRARY
  owsocket
  CACHE INTERNAL "owsocket library"
)

# owsocket lib and dependencies
set(OWSOCKET_LINK_LIBRARIES
  ${OWSOCKET_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWTHREAD_LIBRARY}
)

include_directories(
  ${OWSOCKET_INCLUDE_DIRS}
)

#sources
set(owsocket_SRCS
  src/ServerSocket.cpp
  src/ClientSocket.cpp
)

add_library(${OWSOCKET_LIBRARY} STATIC ${owsocket_SRCS})

target_link_libraries(${OWSOCKET_LINK_LIBRARIES})

