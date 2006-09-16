project(amrwb)

# needed include directories to build amrwb
# saves the variable in internal cache for later use
set(AMRWB_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "amrwb include directories"
)

# amrwb lib and dependencies
# saves the variable in internal cache for later use
set(AMRWB_LIBRARIES
  amrwb
  CACHE INTERNAL "amrwb libraries"
)

include_directories(
  ${AMRWB_INCLUDE_DIRS}
)

set(amrwb_SRCS
  dec_acelp.c
  dec_dtx.c
  dec_gain.c
  dec_if.c
  dec_lpc.c
  dec_main.c
  dec_rom.c
  dec_util.c
  enc_acelp.c
  enc_dtx.c
  enc_gain.c
  enc_if.c
  enc_lpc.c
  enc_main.c
  enc_rom.c
  enc_util.c
  if_rom.c
)

add_library(amrwb STATIC ${amrwb_SRCS})

target_link_libraries(${AMRWB_LIBRARIES})

set_source_files_properties(
  PROPERTIES
    COMPILE_FLAGS
      ${PHAPI_OPTIMIZATION_FLAGS}
)

