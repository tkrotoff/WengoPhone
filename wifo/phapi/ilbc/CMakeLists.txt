project(ilbc)

# needed include directories to build ilbc
# saves the variable in internal cache for later use
set(ILBC_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "ilbc include directories"
)

# ilbc lib and dependencies
# saves the variable in internal cache for later use
set(ILBC_LIBRARIES
  ilbc
  CACHE INTERNAL "ilbc libraries"
)

include_directories(
  ${ILBC_INCLUDE_DIRS}
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

add_library(ilbc STATIC ${ilbc_SRCS})

target_link_libraries(${ILBC_LIBRARIES})

set_source_files_properties(
  PROPERTIES
    COMPILE_FLAGS
      ${PHAPI_OPTIMIZATION_FLAGS}
)

