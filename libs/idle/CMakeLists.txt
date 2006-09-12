project(owidle)

include(${QT_USE_FILE})

# needed include directories to build owidle
# saves the variable in internal cache for later use
set(OWIDLE_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWQTUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owidle include directories"
)

# owidle lib and dependencies
# saves the variable in internal cache for later use
set(OWIDLE_LIBRARIES
  owidle
  ${OWQTUTIL_LIBRARIES}
  CACHE INTERNAL "owidle libraries"
)

include_directories(
  ${OWIDLE_INCLUDE_DIRS}
)

set(owidle_SRCS
  src/Idle.cpp
)

qt4_automoc(${owidle_SRCS})

add_library(owidle STATIC ${owidle_SRCS})

target_link_libraries(${OWIDLE_LIBRARIES})

