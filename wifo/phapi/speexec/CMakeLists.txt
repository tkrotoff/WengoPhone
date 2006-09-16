project(speexec)

# needed include directories to build speexec
# saves the variable in internal cache for later use
set(SPEEXEC_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "speexec include directories"
)

# speexec lib and dependencies
# saves the variable in internal cache for later use
set(SPEEXEC_LIBRARIES
  speexec
  CACHE INTERNAL "speexec libraries"
)

include_directories(
  ${SPEEXEC_INCLUDE_DIRS}
)

set(speexec_SRCS
  src/spxec.c
  src/spxecfft.c
  src/spxecfftwrap.c
)

add_library(speexec STATIC ${speexec_SRCS})

target_link_libraries(${SPEEXEC_LIBRARIES})

