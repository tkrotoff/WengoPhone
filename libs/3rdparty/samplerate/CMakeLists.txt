project(samplerate)

# needed include directories to build samplerate
# saves the variable in internal cache for later use
set(SAMPLERATE_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "samplerate include directories"
)

set(SAMPLERATE_LIBRARY
  samplerate
  CACHE INTERNAL "samplerate library"
)

# samplerate lib and dependencies
set(SAMPLERATE_LINK_LIBRARIES
  ${SAMPLERATE_LIBRARY}
)

set(samplerate_SRCS
  src/samplerate.c
  src/src_linear.c
  src/src_sinc.c
  src/src_zoh.c
)

include_directories(
  ${SAMPLERATE_INCLUDE_DIRS}
)

add_library(${SAMPLERATE_LIBRARY} STATIC ${samplerate_SRCS})

target_link_libraries(${SAMPLERATE_LINK_LIBRARIES})

