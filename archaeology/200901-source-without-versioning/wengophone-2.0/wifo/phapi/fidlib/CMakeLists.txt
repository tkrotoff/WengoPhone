project(fidlib)

# needed include directories to build fidlib
# saves the variable in internal cache for later use
set(FIDLIB_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "fidlib include directories"
)

set(FIDLIB_LIBRARY
  fidlib
  CACHE INTERNAL "fidlib library"
)

# fidlib lib and dependencies
set(FIDLIB_LINK_LIBRARIES
  ${FIDLIB_LIBRARY}
)

set(fidlib_SRCS
  src/fidlib.c
)

if (WIN32)
  add_definitions(
    -DT_MSVC
  )
endif (WIN32)

include_directories(
  ${FIDLIB_INCLUDE_DIRS}
)

add_library(${FIDLIB_LIBRARY} STATIC ${fidlib_SRCS})

target_link_libraries(${FIDLIB_LINK_LIBRARIES})

