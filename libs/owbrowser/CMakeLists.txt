project(owbrowser)

#Qt4 dependency
find_package(Qt4 REQUIRED)

# needed include directories to build owbrowser
# saves the variable in internal cache for later use
set(OWBROWSER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWQTUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owbrowser include directories"
)

# owbrowser lib and dependencies
# saves the variable in internal cache for later use
set(OWBROWSER_LIBRARIES
  owbrowser
  ${OWQTUTIL_LIBRARIES}
  CACHE INTERNAL "owbrowser libraries"
)

include_directories(
  ${OWBROWSER_INCLUDE_DIRS}
)

set(owbrowser_SRCS
  QtBrowser.cpp
)

qt4_automoc(${owbrowser_SRCS})

add_library(owbrowser STATIC ${owbrowser_SRCS})

target_link_libraries(${OWBROWSER_LIBRARIES})
