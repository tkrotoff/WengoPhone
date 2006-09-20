project(speexec)

# needed include directories to build speexec
# saves the variable in internal cache for later use
set(SPEEXEC_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "speexec include directories"
)

set(SPEEXEC_LIBRARY
  speexec
  CACHE INTERNAL "speexec library"
)

# speexec lib and dependencies
set(SPEEXEC_LINK_LIBRARIES
  ${SPEEXEC_LIBRARY}
)

set(speexec_SRCS
  src/spxec.c
  src/spxecfft.c
  src/spxecfftwrap.c
)

include_directories(
  ${SPEEXEC_INCLUDE_DIRS}
)

add_library(${SPEEXEC_LIBRARY} STATIC ${speexec_SRCS})

target_link_libraries(${SPEEXEC_LINK_LIBRARIES})

