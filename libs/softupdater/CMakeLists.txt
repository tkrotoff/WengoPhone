project(owsoftupdater)

# needed include directories to build owsoftupdater
# saves the variable in internal cache for later use
set(OWSOFTUPDATER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWHTTP_INCLUDE_DIRS}
  CACHE INTERNAL "owsoftupdater include directories"
)

# owsoftupdater lib and dependencies
# saves the variable in internal cache for later use
set(OWSOFTUPDATER_LIBRARIES
  owsoftupdater
  ${OWHTTP_LIBRARIES}
  CACHE INTERNAL "owsoftupdater libraries"
)

include_directories(
  ${OWSOFTUPDATER_INCLUDE_DIRS}
)

set(owsoftupdater_SRCS
  src/SoftUpdater.cpp
)

add_library(owsoftupdater STATIC ${owsoftupdater_SRCS})

target_link_libraries(${OWSOFTUPDATER_LIBRARIES})

