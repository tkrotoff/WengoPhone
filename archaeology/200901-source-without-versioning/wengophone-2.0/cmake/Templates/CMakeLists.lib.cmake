project(<PROJECTNAME>)

# needed include directories to build <PROJECTNAME>
# saves the variable in internal cache for later use
set(<LIBNAME>_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OTHERPOJECT_INCLUDE_DIRS}
  CACHE INTERNAL "<PROJECTNAME> include directories"
)

# <PROJECTNAME> lib and dependencies
# saves the variable in internal cache for later use
set(<LIBNAME>_LIBRARIES
  <PROJECTNAME>
  ${OTHERPROJECT_LIBRARIES}
  CACHE INTERNAL "<PROJECTNAME> libraries"
)

include_directories(
  ${<LIBNAME>_INCLUDE_DIRS}
)

set(<PROJECTNAME>_SRCS
  files.c
)

add_library(<PROJECTNAME> STATIC ${<PROJECTNAME>_SRCS})

target_link_libraries(${<LIBNAME>_LIBRARIES})

