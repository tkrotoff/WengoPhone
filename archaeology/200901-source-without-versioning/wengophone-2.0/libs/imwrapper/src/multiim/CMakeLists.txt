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
set(OWMULTIIMWRAPPER_LINK_LIBRARIES
  ${OWMULTIIMWRAPPER_LIBRARY}
  ${OWPHAPIWRAPPER_LIBRARY}
  ${OWGAIMWRAPPER_LIBRARY}
)

set(owmultiimwrapper_SRCS
  MultiIMFactory.cpp
)

include_directories(
  ${OWMULTIIMWRAPPER_INCLUDE_DIRS}
)

add_library(${OWMULTIIMWRAPPER_LIBRARY} STATIC ${owmultiimwrapper_SRCS})

target_link_libraries(${OWMULTIIMWRAPPER_LINK_LIBRARIES})

