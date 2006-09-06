project(owtrayicon)

#Qt4 dependency
find_package(Qt4 REQUIRED)

# needed include directories to build owtrayicon
# saves the variable in internal cache for later use
set(OWTRAYICON_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  CACHE INTERNAL "owtrayicon include directories"
)

# owtrayicon lib and dependencies
# saves the variable in internal cache for later use
set(OWTRAYICON_LIBRARIES
  owtrayicon
  CACHE INTERNAL "owtrayicon libraries"
)

include_directories(
  ${OWTRAYICON_INCLUDE_DIRS}
)

set(owtrayicon_SRCS
  trayicon.cpp
)

if (WIN32)
  set(owtrayicon_SRCS ${owtrayicon_SRCS}
    trayicon_win.cpp
  )
endif (WIN32)

# TODO: cygwin reports as UNIX too.
if (UNIX)
  if (APPLE)
    set(owtrayicon_SRCS ${owtrayicon_SRCS}
      trayicon_mac.cpp
    )
  else (APPLE)
    set(owtrayicon_SRCS ${owtrayicon_SRCS}
      trayicon_x11.cpp
    )
  endif (APPLE)
endif (UNIX)

qt4_automoc(${owtrayicon_SRCS})

add_library(owtrayicon STATIC ${owtrayicon_SRCS})

