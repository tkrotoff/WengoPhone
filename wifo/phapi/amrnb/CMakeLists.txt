project(amrnb)

# needed include directories to build amrnb
# saves the variable in internal cache for later use
set(AMRNB_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "amrnb include directories"
)

# amrnb lib and dependencies
# saves the variable in internal cache for later use
set(AMRNB_LIBRARIES
  amrnb
  CACHE INTERNAL "amrnb libraries"
)

set(amrnb_SRCS
  interf_dec.c
  interf_enc.c
  sp_dec.c
  sp_enc.c
)

include_directories(
  ${AMRNB_INCLUDE_DIRS}
)

add_library(amrnb STATIC ${amrnb_SRCS})

target_link_libraries(${AMRNB_LIBRARIES})

macro_add_compile_flags(amrnb
  ${PHAPI_OPTIMIZATION_FLAGS}
)

