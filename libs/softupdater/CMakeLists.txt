project(owsoftupdater)

# needed include directories to build owsoftupdater
# saves the variable in internal cache for later use
set(OWSOFTUPDATER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  ${OWHTTP_INCLUDE_DIRS}
  CACHE INTERNAL "owsoftupdater include directories"
)

set(OWSOFTUPDATER_LIBRARY
  owsoftupdater
  CACHE INTERNAL "owsoftupdater libraries"
)

# owsoftupdater lib and dependencies
set(OWSOFTUPDATER_LINK_LIBRARIES
  ${OWSOFTUPDATER_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWHTTP_LIBRARY}
  CACHE INTERNAL "owsoftupdater libraries"
)

set(owsoftupdater_SRCS
  src/SoftUpdater.cpp
)

include_directories(
  ${OWSOFTUPDATER_INCLUDE_DIRS}
)

add_library(${OWSOFTUPDATER_LIBRARY} STATIC ${owsoftupdater_SRCS})

target_link_libraries(${OWSOFTUPDATER_LINK_LIBRARIES})

