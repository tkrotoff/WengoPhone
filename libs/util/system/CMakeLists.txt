project(owsystem)

# needed include directories to build owsystem
# saves the variable in internal cache for later use
set(OWSYSTEM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  CACHE INTERNAL "owsystem include directories"
)

# owsystem lib and dependencies
# saves the variable in internal cache for later use
set(OWSYSTEM_LIBRARIES
  owsystem
  ${OWUTIL_LIBRARIES}
  CACHE INTERNAL "owsystem libraries"
)

include_directories(
  ${OWSYSTEM_INCLUDE_DIRS}
)

#sources
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

    macro_add_compile_flags(owsystem_SRCS
      "-x objective-c++"
    )
  else (APPLE)
    set(owsystem_SRCS ${owsystem_SRCS}
      src/unix/Processes.cpp
      src/null/Startup.cpp
    )
  endif (APPLE)
endif (UNIX)

add_library(owsystem STATIC ${owsystem_SRCS})

target_link_libraries(${OWSYSTEM_LIBRARIES})
