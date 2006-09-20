project(owbrowser)

# needed include directories to build owbrowser
# saves the variable in internal cache for later use
set(OWBROWSER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWCUTIL_INCLUDE_DIRS}
  ${OWUTIL_INCLUDE_DIRS}
  ${OWQTUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owbrowser include directories"
)

set(OWBROWSER_LIBRARY
  owbrowser
  CACHE INTERNAL "owbrowser libraries"
)
# owbrowser lib and dependencies
set(OWBROWSER_LINK_LIBRARIES
  ${OWBROWSER_LIBRARY}
  ${OWCUTIL_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWQTUTIL_LIBRARY}
)

set(owbrowser_SRCS
  QtBrowser.cpp
)

qt4_automoc(${owbrowser_SRCS})

include_directories(
  ${OWBROWSER_INCLUDE_DIRS}
)

add_library(${OWBROWSER_LIBRARY} STATIC ${owbrowser_SRCS})

target_link_libraries(${OWBROWSER_LINK_LIBRARIES})
