project(samplerate)

# needed include directories to build samplerate
# saves the variable in internal cache for later use
set(SAMPLERATE_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "samplerate include directories"
)

# samplerate lib and dependencies
# saves the variable in internal cache for later use
set(SAMPLERATE_LIBRARIES
  samplerate
  CACHE INTERNAL "samplerate libraries"
)

include_directories(
  ${SAMPLERATE_INCLUDE_DIRS}
)

set(samplerate_SRCS
  src/samplerate.c
  src/src_linear.c
  src/src_sinc.c
  src/src_zoh.c
)

add_library(samplerate STATIC ${samplerate_SRCS})

target_link_libraries(${SAMPLERATE_LIBRARIES})

