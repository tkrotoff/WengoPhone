project(yealinkwenbox)

find_package(Boost REQUIRED)

# find precompiled ylusbtel
find_library(YLUSBTEL_LIBRARY
  NAMES
    YLUSBTEL
  PATH
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# hack because the find_library does not work, why? no idea...
set(YLUSBTEL_LIBRARY
  ${CMAKE_CURRENT_SOURCE_DIR}/YLUSBTEL
)

# needed include directories to build yealinkwenbox
set(YEALINKWENBOX_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OWUTIL_INCLUDE_DIRS}
  ${OWWENBOX_INCLUDE_DIRS}
)

set(YEALINKWENBOX_LIBARY
  wenboxplugin
)

# yealinkwenbox lib and dependencies
set(YEALINKWENBOX_LIBRARIES
  ${YEALINKWENBOX_LIBARY}
  ${OWUTIL_LIBRARY}
  ${YLUSBTEL_LIBRARY}
  ${OWWENBOX_LIBRARY}
)

include_directories(
  ${YEALINKWENBOX_INCLUDE_DIRS}
)

set(yealinkwenbox_SRCS
  YealinkWenbox.cpp
)

macro_add_plugin(${YEALINKWENBOX_LIBARY} ${yealinkwenbox_SRCS})

target_link_libraries(${YEALINKWENBOX_LIBRARIES})

install(
  TARGETS
    ${YEALINKWENBOX_LIBARY}
  DESTINATION
    ${PLUGIN_INSTALL_DIR}
)
