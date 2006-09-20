project(owidle)

include(${QT_USE_FILE})

# needed include directories to build owidle
# saves the variable in internal cache for later use
set(OWIDLE_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
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
  ${OWQTUTIL_LIBRARIES}
  ${QT_CORE_LIBRARY}
)

set(owidle_SRCS
  src/Idle.cpp
)

qt4_automoc(${owidle_SRCS})

include_directories(
  ${OWIDLE_INCLUDE_DIRS}
)

add_library(${OWIDLE_LIBRARY} STATIC ${owidle_SRCS})

target_link_libraries(${OWIDLE_LINK_LIBRARIES})

