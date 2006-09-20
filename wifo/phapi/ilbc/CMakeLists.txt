project(ilbc)

# needed include directories to build ilbc
# saves the variable in internal cache for later use
set(ILBC_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "ilbc include directories"
)

set(ILBC_LIBRARY
  ilbc
  CACHE INTERNAL "ilbc library"
)

# ilbc lib and dependencies
set(ILBC_LINK_LIBRARIES
  ${ILBC_LIBRARY}
)

set(ilbc_SRCS
  anaFilter.c
  constants.c
  createCB.c
  doCPLC.c
  enhancer.c
  filter.c
  FrameClassify.c
  gainquant.c
  getCBvec.c
  helpfun.c
  hpInput.c
  hpOutput.c
  iCBConstruct.c
  iCBSearch.c
  iLBC_decode.c
  iLBC_encode.c
  LPCdecode.c
  LPCencode.c
  lsf.c
  packing.c
  StateConstructW.c
  StateSearchW.c
  syntFilter.c
)

include_directories(
  ${ILBC_INCLUDE_DIRS}
)

add_library(${ILBC_LIBRARY} STATIC ${ilbc_SRCS})

target_link_libraries(${ILBC_LINK_LIBRARIES})

macro_add_compile_flags(${ILBC_LIBRARY}
  ${PHAPI_OPTIMIZATION_FLAGS}
)

