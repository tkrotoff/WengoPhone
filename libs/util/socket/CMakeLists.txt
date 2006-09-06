project(owsocket)

# needed include directories to build owsocket
# saves the variable in internal cache for later use
set(OWSOCKET_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWTHREAD_INCLUDE_DIRS}
  CACHE INTERNAL "owsocket include directories"
)

# owsocket lib and dependencies
# saves the variable in internal cache for later use
set(OWSOCKET_LIBRARIES
  owsocket
  ${OWTHREAD_LIBRARIES}
  CACHE INTERNAL "owsocket libraries"
)

include_directories(
  ${OWSOCKET_INCLUDE_DIRS}
)

#sources
set(owsocket_SRCS
  src/ServerSocket.cpp
  src/ClientSocket.cpp
)

add_library(owsocket STATIC ${owsocket_SRCS})

target_link_libraries(${OWSOCKET_LIBRARIES})

