project(httptunnel)

# needed include directories to build httptunnel
# saves the variable in internal cache for later use
set(HTTPTUNNEL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWCURL_INCLUDE_DIRS}
  CACHE INTERNAL "httptunnel include directories"
)

# httptunnel lib and dependencies
# saves the variable in internal cache for later use
set(HTTPTUNNEL_LIBRARIES
  httptunnel
  ${OWCURL_LIBRARIES}
  CACHE INTERNAL "httptunnel libraries"
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

add_library(httptunnel STATIC ${httptunnel_SRCS})

target_link_libraries(${HTTPTUNNEL_LIBRARIES})

