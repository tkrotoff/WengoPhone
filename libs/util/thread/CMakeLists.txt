project(owthread)

# needed include directories to build owthread
# saves the variable in internal cache for later use
set(OWTHREAD_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  CACHE INTERNAL "owthread include directories"
)

set(OWTHREAD_LIBRARY
  owthread
  CACHE INTERNAL "owthread library"
)

# owthread lib and dependencies
set(OWTHREAD_LINK_LIBRARIES
  ${OWTHREAD_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${BOOST_THREAD_LIBRARY}
  CACHE INTERNAL "owthread libraries"
)

set(owthread_SRCS
  src/Timer.cpp
  src/Thread.cpp
)

include_directories(
  ${OWTHREAD_INCLUDE_DIRS}
)

add_library(${OWTHREAD_LIBRARY} STATIC ${owthread_SRCS})

target_link_libraries(${OWTHREAD_LINK_LIBRARIES})

