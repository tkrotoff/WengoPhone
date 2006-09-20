project(amrnb)

# needed include directories to build amrnb
# saves the variable in internal cache for later use
set(AMRNB_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "amrnb include directories"
)

set(AMRNB_LIBRARY
  amrnb
  CACHE INTERNAL "amrnb library"
)

# amrnb lib and dependencies
set(AMRNB_LINK_LIBRARIES
  ${AMRNB_LIBRARY}
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

add_library(${AMRNB_LIBRARY} STATIC ${amrnb_SRCS})

target_link_libraries(${AMRNB_LINK_LIBRARIES})

macro_add_compile_flags(${AMRNB_LIBRARY}
  ${PHAPI_OPTIMIZATION_FLAGS}
)

