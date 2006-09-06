project(owthread)

# needed include directories to build owthread
# saves the variable in internal cache for later use
set(OWTHREAD_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owthread include directories"
)

# owthread lib and dependencies
# saves the variable in internal cache for later use
set(OWTHREAD_LIBRARIES
  owthread
  ${OWUTIL_LIBRARIES}
  CACHE INTERNAL "owthread libraries"
)

include_directories(
  ${OWTHREAD_INCLUDE_DIRS}
)
#owutil dependency
link_directories(${owutil_BINARY_DIR})

#includes
set(owthread_INCLUDE_DIR
  ${owthread_SOURCE_DIR}/include
  ${owutil_INCLUDE_DIR}
  CACHE INTERNAL ""
)

include_directories(${owthread_INCLUDE_DIR})

#sources
set(owthread_SRCS
  src/Timer.cpp
  src/Thread.cpp
)

#owthread
add_library(owthread STATIC ${owthread_SRCS})

target_link_libraries(${OWTHREAD_LIBRARIES})

