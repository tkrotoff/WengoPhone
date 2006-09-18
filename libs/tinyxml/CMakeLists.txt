project(tinyxml)

# needed include directories to build tinyxml
# saves the variable in internal cache for later use
set(TINYXML_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  CACHE INTERNAL "tinyxml include directories"
)

# tinyxml lib and dependencies
# saves the variable in internal cache for later use
set(TINYXML_LIBRARIES
  tinyxml
  CACHE INTERNAL "tinyxml libraries"
)

include_directories(
  ${TINYXML_INCLUDE_DIRS}
)

include_directories(${TINYXML_INCLUDE_DIRS})

set(tinyxml_SRCS
  tinyxml.cpp
  tinyxmlerror.cpp
  tinyxmlparser.cpp
)

add_library(tinyxml STATIC ${tinyxml_SRCS})
