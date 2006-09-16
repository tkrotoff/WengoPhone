project(gsm)

# needed include directories to build gsm
# saves the variable in internal cache for later use
set(GSM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "gsm include directories"
)

# gsm lib and dependencies
# saves the variable in internal cache for later use
set(GSM_LIBRARIES
  gsm
  CACHE INTERNAL "gsm libraries"
)

include_directories(
  ${GSM_INCLUDE_DIRS}
)

set(gsm_SRCS
  add.c
  code.c
  debug.c
  decode.c
  gsm_create.c
  gsm_decode.c
  gsm_destroy.c
  gsm_encode.c
  gsm_explode.c
  gsm_implode.c
  gsm_option.c
  gsm_print.c
  long_term.c
  lpc.c
  preprocess.c
  rpe.c
  short_term.c
  table.c
)

add_library(gsm STATIC ${gsm_SRCS})

target_link_libraries(${GSM_LIBRARIES})

