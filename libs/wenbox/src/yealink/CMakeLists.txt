project(owwenboxplugin)

find_package(Boost REQUIRED)

# find precompiled ylusbtel
find_library(YLUSBTEL_LIBRARIES
  NAMES
    ylusbtel
  PATH
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# needed include directories to build owwenboxplugin
# saves the variable in internal cache for later use
set(OWWENBOXPLUGIN_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWWENBOX_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  CACHE INTERNAL "owwenboxplugin include directories"
)

# owwenboxplugin lib and dependencies
# saves the variable in internal cache for later use
set(OWWENBOXPLUGIN_LIBRARIES
  owwenboxplugin
  ${YLUSBTEL_LIBRARIES}
  ${OWWENBOX_LIBRARIES}
  CACHE INTERNAL "owwenboxplugin libraries"
)

include_directories(
  ${OWWENBOXPLUGIN_INCLUDE_DIRS}
)

set(owwenboxplugin_SRCS
  YealinkWenbox.cpp
)

add_library(owwenboxplugin SHARED ${owwenboxplugin_SRCS})

target_link_libraries(${OWWENBOXPLUGIN_LIBRARIES})

