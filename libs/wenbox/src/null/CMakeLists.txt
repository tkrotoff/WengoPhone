project(ownullwenbox)

find_package(Boost REQUIRED)

# needed include directories to build ownullwenbox
# saves the variable in internal cache for later use
set(OWNULLWENBOX_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
  CACHE INTERNAL "ownullwenbox include directories"
)

# ownullwenbox lib and dependencies
# saves the variable in internal cache for later use
set(OWNULLWENBOX_LIBRARIES
  ownullwenbox
  ${OWWENBOX_LIBRARIES}
  CACHE INTERNAL "ownullwenbox libraries"
)

include_directories(
  ${OWNULLWENBOX_INCLUDE_DIRS}
)

set(ownullwenbox_SRCS
  NullWenbox.cpp
)

add_library(ownullwenbox STATIC ${ownullwenbox_SRCS})

target_link_libraries(ownullwenbox owwenbox)

