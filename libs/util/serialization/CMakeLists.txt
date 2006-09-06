project(owserialization)

# needed include directories to build owserialization
# saves the variable in internal cache for later use
set(OWSERIALIZATION_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWCUTIL_INCLUDE_DIRS}
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owserialization include directories"
)

# OWSERIALIZATION lib and dependencies
# saves the variable in internal cache for later use
set(OWSERIALIZATION_LIBRARIES
  owserialization
  ${OWCUTIL_LIBRARIES}
  ${OWUTIL_LIBRARIES}
  CACHE INTERNAL "owserialization libraries"
)

include_directories(
  ${OWSERIALIZATION_INCLUDE_DIRS}
)

set(owserialization_SRCS
  src/fake.cpp
)

add_library(owserialization STATIC ${owserialization_SRCS})

target_link_libraries(${OWSERIALIZATION_LIBRARIES})

