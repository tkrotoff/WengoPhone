project(httptunnel)

# needed include directories to build httptunnel
# saves the variable in internal cache for later use
set(HTTPTUNNEL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWCURL_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIRS}
  CACHE INTERNAL "httptunnel include directories"
)

set(HTTPTUNNEL_LIBRARY
  httptunnel
  CACHE INTERNAL "httptunnel library"
)

# httptunnel lib and dependencies
set(HTTPTUNNEL_LINK_LIBRARIES
  ${HTTPTUNNEL_LIBRARY}
  ${OWCURL_LIBRARY}
  ${OPENSSL_LIBRARIES}
)

set(HTTPTUNNEL_DEFINITIONS
  -DHT_USE_SSL
)

if (WIN32)
  set(HTTPTUNNEL_DEFINITIONS
    ${HTTPTUNNEL_DEFINITIONS}
    -DWIN32
  )
endif (WIN32)

include_directories(
  ${HTTPTUNNEL_INCLUDE_DIRS}
)

add_definitions(
  ${HTTPTUNNEL_DEFINITIONS}
)

set(httptunnel_SRCS
  httptunnel.c
)

add_library(${HTTPTUNNEL_LIBRARY} STATIC ${httptunnel_SRCS})

target_link_libraries(${HTTPTUNNEL_LINK_LIBRARIES})

