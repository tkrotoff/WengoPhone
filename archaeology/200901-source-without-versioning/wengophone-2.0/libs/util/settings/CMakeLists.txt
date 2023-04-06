project(owsettings)

# needed include directories to build owsettings
# saves the variable in internal cache for later use
set(OWSETTINGS_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWSERIALIZATION_INCLUDE_DIRS}
  ${OWTHREAD_INCLUDE_DIRS}
  ${TINYXML_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  CACHE INTERNAL "owsettings include directories"
)

set(OWSETTINGS_LIBRARY
  owsettings
  CACHE INTERNAL "owsettings library"
)

# owsettings lib and dependencies
set(OWSETTINGS_LINK_LIBRARIES
  ${OWSETTINGS_LIBRARY}
  ${OWSERIALIZATION_LIBRARY}
  ${OWTHREAD_LIBRARY}
  ${TINYXML_LIBRARY}
)

set(owsettings_SRCS
  src/Settings.cpp
  src/AutomaticSettings.cpp
  src/StringListXMLSerializer.cpp
  src/SettingsXMLSerializer.cpp
)

include_directories(
  ${OWSETTINGS_INCLUDE_DIRS}
)

add_library(${OWSETTINGS_LIBRARY} STATIC ${owsettings_SRCS})

target_link_libraries(${OWSETTINGS_LINK_LIBRARIES})

