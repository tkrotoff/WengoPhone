project(owsystem)

# needed include directories to build owsystem
# saves the variable in internal cache for later use
set(OWSYSTEM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owsystem include directories"
)

set(OWSYSTEM_LIBRARY
  owsystem
  CACHE INTERNAL "owsystem libraries"
)

# owsystem lib and dependencies
set(OWSYSTEM_LINK_LIBRARIES
  ${OWSYSTEM_LIBRARY}
  ${OWUTIL_LIBRARY}
)

set(owsystem_SRCS
  src/RegisterProtocol.cpp
)

if (WIN32)
  set(owsystem_SRCS ${owsystem_SRCS}
    src/win/Startup.cpp
    src/win/Processes.cpp
    src/win/WindowsVersion.c
    src/win/WindowsRegistry.cpp
  )
endif (WIN32)

if (UNIX)
  if (APPLE)
    set(owsystem_SRCS ${owsystem_SRCS}
      src/null/Processes.cpp
      src/mac/Startup.cpp
    )
  else (APPLE)
    set(owsystem_SRCS ${owsystem_SRCS}
      src/unix/Processes.cpp
      src/null/Startup.cpp
    )
  endif (APPLE)
endif (UNIX)

include_directories(
  ${OWSYSTEM_INCLUDE_DIRS}
)

add_library(${OWSYSTEM_LIBRARY} STATIC ${owsystem_SRCS})

if (APPLE)
    macro_add_compile_flags(${OWSYSTEM_LIBRARY}
      "-x objective-c++"
    )
endif (APPLE)

target_link_libraries(${OWSYSTEM_LINK_LIBRARIES})

