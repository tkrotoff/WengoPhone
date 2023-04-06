project(owshlibloader)

# needed include directories to build owshlibloader
# saves the variable in internal cache for later use
set(OWSHLIBLOADER_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owshlibloader include directories"
)

set(OWSHLIBLOADER_LIBRARY
  owshlibloader
  CACHE INTERNAL "owshlibloader library"
)

# owshlibloader lib and dependencies
set(OWSHLIBLOADER_LINK_LIBRARIES
  ${OWSHLIBLOADER_LIBRARY}
  ${OWUTIL_LIBRARY}
)

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

include_directories(
  ${OWSHLIBLOADER_INCLUDE_DIRS}
)

add_library(${OWSHLIBLOADER_LIBRARY} STATIC ${owshlibloader_SRCS})

target_link_libraries(${OWSHLIBLOADER_LINK_LIBRARIES})

