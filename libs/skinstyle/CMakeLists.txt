project(skinstyle)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

# needed include directories to build skinstyle
# saves the variable in internal cache for later use
set(SKINSTYLE_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWQTUTIL_INCLUDE_DIRS}
  ${QT_INCLUDE_DIR}
  CACHE INTERNAL "skinstyle include directories"
)

# skinstyle lib and dependencies
# saves the variable in internal cache for later use
set(SKINSTYLE_LIBRARIES
  skinstyle
  ${OWQTUTIL_LIBRARIES}
  CACHE INTERNAL "skinstyle libraries"
)

include_directories(
  ${SKINSTYLE_INCLUDE_DIRS}
)

set(skinstyle_SRCS
  src/SkinStyle.cpp
  src/SkinInfoReader.cpp
  src/SkinWidget.cpp
  src/SkinWindow.cpp
  src/SkinButton.cpp
)

qt4_automoc(${skinstyle_SRCS})

add_library(skinstyle STATIC ${skinstyle_SRCS})

target_link_libraries(${SKINSTYLE_LIBRARIES})

