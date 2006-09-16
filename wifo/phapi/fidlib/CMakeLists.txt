project(fidlib)

# needed include directories to build fidlib
# saves the variable in internal cache for later use
set(FIDLIB_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "fidlib include directories"
)

# fidlib lib and dependencies
# saves the variable in internal cache for later use
set(FIDLIB_LIBRARIES
  fidlib
  CACHE INTERNAL "fidlib libraries"
)

if (WIN32)
  add_definitions(
    -DT_MSVC
  )
endif (WIN32)

include_directories(
  ${FIDLIB_INCLUDE_DIRS}
)

set(fidlib_SRCS
  src/fidlib.c
)

add_library(fidlib STATIC ${fidlib_SRCS})

target_link_libraries(${FIDLIB_LIBRARIES})

