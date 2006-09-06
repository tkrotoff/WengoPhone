project(owshlibloader)

# needed include directories to build owshlibloader
# saves the variable in internal cache for later use
set(OWSHLIBLOADER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owshlibloader include directories"
)

# owshlibloader lib and dependencies
# saves the variable in internal cache for later use
set(OWSHLIBLOADER_LIBRARIES
  owshlibloader
  ${OWUTIL_LIBRARIES}
  CACHE INTERNAL "owshlibloader libraries"
)

include_directories(
  ${OWSHLIBLOADER_INCLUDE_DIRS}
)
#sources
set(owshlibloader_SRCS
  src/SharedLibLoader.cpp
)

if (WIN32)
  set(owshlibloader_SRCS ${owshlibloader_SRCS}
    src/win32/Win32SharedLibLoader.cpp
  )
endif (WIN32)

if (UNIX)
  set(owshlibloader_SRCS ${owshlibloader_SRCS}
    src/posix/PosixSharedLibLoader.cpp
  )
endif (UNIX)

#owshlibloader
add_library(owshlibloader STATIC ${owshlibloader_SRCS})

target_link_libraries(${OWSHLIBLOADER_LIBRARIES})
