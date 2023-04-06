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

set(NETLIB_LIBRARY
  netlib
  CACHE INTERNAL "netlib library"
)

# netlib lib and dependencies
set(NETLIB_LINK_LIBRARIES
  ${NETLIB_LIBRARY}
  ${OWCUTIL_LIBRARY}
  ${HTTPTUNNEL_LIBRARY}
)

if (MSVC)
  set(NETLIB_LINK_LIBRARIES
    ${NETLIB_LINK_LIBRARIES}
    user32
    shell32
    ws2_32
    advapi32
    wininet
    urlmon
    CACHE INTERNAL "netlib libraries"
  )
endif (MSVC)

set(netlib_SRCS
  src/netlib.cpp
  src/netlib_util.cpp
  src/stun_func.cpp
  src/udp_func.cpp
  src/bugfix.cpp
)

include_directories(
  ${NETLIB_INCLUDE_DIRS}
)

add_library(${NETLIB_LIBRARY} STATIC ${netlib_SRCS})

target_link_libraries(${NETLIB_LINK_LIBRARIES})

