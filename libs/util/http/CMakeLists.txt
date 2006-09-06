project(owhttp)

# needed include directories to build owhttp
# saves the variable in internal cache for later use
set(OWHTTP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWTHREAD_INCLUDE_DIRS}
  CACHE INTERNAL "owhttp include directories"
)

# owhttp lib and dependencies
# saves the variable in internal cache for later use
set(OWHTTP_LIBRARIES
  owhttp
  ${OWTHREAD_LIBRARIES}
  CACHE INTERNAL "owhttp libraries"
)

include_directories(
  ${OWHTTP_INCLUDE_DIRS}
)

#sources
set(owhttp_SRCS
  src/HttpRequest.cpp
  src/null/NullHttpRequest.cpp
  src/curl/CurlHttpRequest.cpp
  #src/qt/QtHttpRequest.cpp
)

add_library(owhttp STATIC ${owhttp_SRCS})

target_link_libraries(${OWTHREAD_LIBRARIES})

