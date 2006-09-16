project(stun)

# needed include directories to build stun
# saves the variable in internal cache for later use
set(STUN_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "stun include directories"
)

# stun lib and dependencies
# saves the variable in internal cache for later use
set(STUN_LIBRARIES
  stun
  CACHE INTERNAL "stun libraries"
)

if (WIN32)
  add_definitions(
    -DWIN32
  )
endif (WIN32)

include_directories(
  ${STUN_INCLUDE_DIRS}
)

set(stun_SRCS
  udp.c
  stun.c
)

add_library(stun STATIC ${stun_SRCS})

target_link_libraries(${STUN_LIBRARIES})

