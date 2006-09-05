project(<LIBNAME>)

# needed include directories to build <LIBNAME>
# saves the variable in internal cache for later use
set(<LIBNAME>_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OTHERPOJECT_INCLUDE_DIRS}
  CACHE INTERNAL "<LIBNAME> include directories"
)

# <LIBNAME> lib and dependencies
# saves the variable in internal cache for later use
set(<LIBNAME>_LIBRARIES
  <LIBNAME>
  ${OTHERPROJECT_LIBRARIES}
  CACHE INTERNAL "<LIBNAME libraries"
)

include_directories(
  ${<LIBNAME>_INCLUDE_DIRS}
)

set(<LIBNAME>_SRCS
  files.c
)

add_library(<LIBNAME> STATIC ${<LIBNAME>_SRCS})

target_link_libraries(${<LIBNAME>_LIBRARIES})

