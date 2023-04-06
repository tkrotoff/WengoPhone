project(owidle)

include(${QT_USE_FILE})

# needed include directories to build owidle
# saves the variable in internal cache for later use
set(OWIDLE_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  ${OWQTUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owidle include directories"
)

set(OWIDLE_LIBRARY
  owidle
  CACHE INTERNAL "owidle library"
)

# owidle lib and dependencies
set(OWIDLE_LINK_LIBRARIES
  ${OWIDLE_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWQTUTIL_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
)

qt4_wrap_cpp(owidle_SRCS
  include/idle/Idle.h
)

set(owidle_SRCS
  ${owidle_SRCS}
  src/Idle.cpp
)

include_directories(
  ${OWIDLE_INCLUDE_DIRS}
)

add_library(${OWIDLE_LIBRARY} STATIC ${owidle_SRCS})

target_link_libraries(${OWIDLE_LINK_LIBRARIES})

