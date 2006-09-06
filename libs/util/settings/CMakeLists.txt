project(owsettings)

# needed include directories to build owsettings
# saves the variable in internal cache for later use
set(OWSETTINGS_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWSERIALIZATION_INCLUDE_DIRS}
  ${OWTHREAD_INCLUDE_DIRS}
  ${TINYXML_INCLUDE_DIRS}
  CACHE INTERNAL "owsettings include directories"
)

# owsettings lib and dependencies
# saves the variable in internal cache for later use
set(OWSETTINGS_LIBRARIES
  owsettings
  ${OWSERIALIZATION_LIBRARIES}
  ${OWTHREAD_LIBRARIES}
  ${TINYXML_LIBRARIES}
  CACHE INTERNAL "owsettings libraries"
)

include_directories(
  ${OWSETTINGS_INCLUDE_DIRS}
)

set(owsettings_SRCS
  src/Settings.cpp
  src/AutomaticSettings.cpp
  src/StringListXMLSerializer.cpp
  src/SettingsXMLSerializer.cpp
)

#owsettings
add_library(owsettings STATIC ${owsettings_SRCS})

#owutil dependency
target_link_libraries(${OWSETTINGS_LIBRARIES})
