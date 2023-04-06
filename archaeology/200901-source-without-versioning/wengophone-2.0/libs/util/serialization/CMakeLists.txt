project(owserialization)

# needed include directories to build owserialization
# saves the variable in internal cache for later use
set(OWSERIALIZATION_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owserialization include directories"
)

set(OWSERIALIZATION_LIBRARY
  owserialization
  CACHE INTERNAL "owserialization library"
)

# OWSERIALIZATION lib and dependencies
# saves the variable in internal cache for later use
set(OWSERIALIZATION_LINK_LIBRARIES
  ${OWSERIALIZATION_LIBRARY}
  ${OWUTIL_LIBRARY}
)

set(owserialization_SRCS
  src/fake.cpp
)

include_directories(
  ${OWSERIALIZATION_INCLUDE_DIRS}
)

add_library(${OWSERIALIZATION_LIBRARY} STATIC ${owserialization_SRCS})

target_link_libraries(${OWSERIALIZATION_LINK_LIBRARIES})

