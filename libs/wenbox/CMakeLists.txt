project(owwenbox)

if (WIN32)
  subdirs(src/yealink)
endif (WIN32)

# needed include directories to build owwenbox
# saves the variable in internal cache for later use
set(OWWENBOX_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWCUTIL_INCLUDE_DIRS}
  ${OWUTIL_INCLUDE_DIRS}
  ${OWSHLIBLOADER_INCLUDE_DIRS}
  CACHE INTERNAL "owwenbox include directories"
)

set(OWWENBOX_LIBRARY
  owwenbox
  CACHE INTERNAL "owwenbox library"
)

# owwenbox lib and dependencies
set(OWWENBOX_LINK_LIBRARIES
  ${OWWENBOX_LIBRARY}
  ${OWCUTIL_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWSHLIBLOADER_LIBRARY}
  CACHE INTERNAL "owwenbox libraries"
)

set(owwenbox_SRCS
  src/Wenbox.cpp
)

include_directories(
  ${OWWENBOX_INCLUDE_DIRS}
)

add_library(${OWWENBOX_LIBRARY} STATIC ${owwenbox_SRCS})

target_link_libraries(${OWWENBOX_LINK_LIBRARIES})

