project(msdirent)

set(MSDIRENT_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "msdirent include directory"
)

set(MSDIRENT_LIBRARY
  msdirent
  CACHE INTERNAL "msdirent library"
)

set(MSDIRENT_LINK_LIBRARIES
  ${MSDIRENT_LIBRARY}
)

set(msdirent_SRCS
  msdirent.c
)

include_directories(${MSDIRENT_INCLUDE_DIRS})

add_library(${MSDIRENT_LIBRARY} STATIC ${msdirent_SRCS})
