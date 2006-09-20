project(owhttp)

# needed include directories to build owhttp
# saves the variable in internal cache for later use
set(OWHTTP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWTHREAD_INCLUDE_DIRS}
  ${OWCURL_INCLUDE_DIRS}
  CACHE INTERNAL "owhttp include directories"
)

set(OWHTTP_LIBRARY
  owhttp
  CACHE INTERNAL "owhttp library"
)

# owhttp lib and dependencies
set(OWHTTP_LINK_LIBRARIES
  ${OWHTTP_LIBRARY}
  ${OWTHREAD_LIBRARY}
  ${OWCURL_LIBRARY}
)

if (WIN32)
  set(OWHTTP_LINK_LIBRARIES
    ${OWHTTP_LINK_LIBRARIES}
    advapi32
    shell32
  )
endif (WIN32)

set(owhttp_SRCS
  src/HttpRequest.cpp
  src/curl/CurlHttpRequest.cpp
  src/null/NullHttpRequest.cpp
  #src/qt/QtHttpRequest.cpp
)

include_directories(
  ${OWHTTP_INCLUDE_DIRS}
)

add_library(${OWHTTP_LIBRARY} STATIC ${owhttp_SRCS})

target_link_libraries(${OWHTTP_LINK_LIBRARIES})

if (APPLE)
  macro_add_link_flags(${OWHTTP_LIBRARY}
    "-framework CoreFoundation"
  )
endif (APPLE)

