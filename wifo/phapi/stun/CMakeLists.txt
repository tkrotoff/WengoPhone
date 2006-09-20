project(stun)

# needed include directories to build stun
# saves the variable in internal cache for later use
set(STUN_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "stun include directories"
)

set(STUN_LIBRARY
  stun
  CACHE INTERNAL "stun library"
)

# stun lib and dependencies
set(STUN_LINK_LIBRARIES
  ${STUN_LIBRARY}
)

if (WIN32)
  add_definitions(
    -DWIN32
  )
endif (WIN32)

set(stun_SRCS
  udp.c
  stun.c
)

include_directories(
  ${STUN_INCLUDE_DIRS}
)

add_library(${STUN_LIBRARY} STATIC ${stun_SRCS})

target_link_libraries(${STUN_LINK_LIBRARIES})

