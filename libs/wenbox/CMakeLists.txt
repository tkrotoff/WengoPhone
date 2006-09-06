project(owwenbox)

# needed include directories to build owwenbox
# saves the variable in internal cache for later use
set(OWWENBOX_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWSHLIBLOADER_INCLUDE_DIRS}
  CACHE INTERNAL "owwenbox include directories"
)

# owwenbox lib and dependencies
# saves the variable in internal cache for later use
set(OWWENBOX_LIBRARIES
  owwenbox
  ${OWSHLIBLOADER_LIBRARIES}
  CACHE INTERNAL "owwenbox libraries"
)

include_directories(
  ${OWWENBOX_INCLUDE_DIRS}
)


#sources
set(owwenbox_SRCS
  src/Wenbox.cpp
)

add_library(owwenbox STATIC ${owwenbox_SRCS})

target_link_libraries(${OWWENBOX_LIBRARIES})

subdirs(src/null)
if (WIN32)
  subdirs(src/yealink)
endif (WIN32)
