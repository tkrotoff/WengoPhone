project(netlib)

# needed include directories to build netlib
# saves the variable in internal cache for later use
set(NETLIB_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWCUTIL_INCLUDE_DIRS}
  ${HTTPTUNNEL_INCLUDE_DIRS}
  CACHE INTERNAL "netlib include directories"
)

# netlib lib and dependencies
# saves the variable in internal cache for later use
set(NETLIB_LIBRARIES
  netlib
  ${OWCUTIL_LIBRARIES}
  ${HTTPTUNNEL_LIBRARIES}
  CACHE INTERNAL "netlib libraries"
)

if (MSVC)
  set(NETLIB_LIBRARIES
    ${NETLIB_LIBRARIES}
    user32
    shell32
    ws2_32
    advapi32
    wininet
    urlmon
    CACHE INTERNAL "netlib libraries"
  )
endif (MSVC)

include_directories(
  ${NETLIB_INCLUDE_DIRS}
)

set(netlib_SRCS
  src/netlib.cpp
  src/netlib_util.cpp
  src/stun_func.cpp
  src/udp_func.cpp
  src/bugfix.cpp
)

add_library(netlib STATIC ${netlib_SRCS})

target_link_libraries(${NETLIB_LIBRARIES})

