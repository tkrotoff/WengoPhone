project(owmultiimwrapper)

# needed include directories to build owmultiimwrapper
# saves the variable in internal cache for later use
set(OWMULTIIMWRAPPER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/multiim
  ${OWPHAPIWRAPPER_INCLUDE_DIRS}
  ${OWGAIMWRAPPER_INCLUDE_DIRS}
  CACHE INTERNAL "owmultiimwrapper include directories"
)

set(OWMULTIIMWRAPPER_LIBRARY
  owmultiimwrapper
  CACHE INTERNAL "owmultiimwrapper library"
)

# owmultiimwrapper lib and dependencies
# saves the variable in internal cache for later use
set(OWMULTIIMWRAPPER_LIBRARIES
  owmultiimwrapper
  ${OWPHAPIWRAPPER_LIBRARIES}
  ${OWGAIMWRAPPER_LIBRARIES}
  CACHE INTERNAL "owmultiimwrapper libraries"
)

set(owmultiimwrapper_SRCS
  MultiIMFactory.cpp
)

include_directories(
  ${OWMULTIIMWRAPPER_INCLUDE_DIRS}
)


add_library(owmultiimwrapper STATIC ${owmultiimwrapper_SRCS})

target_link_libraries(${OWMULTIIMWRAPPER_LIBRARIES})

