project(owtrayicon)

# needed include directories to build owtrayicon
# saves the variable in internal cache for later use
set(OWTRAYICON_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${QT_INCLUDE_DIR}
  CACHE INTERNAL "owtrayicon include directories"
)

set(OWTRAYICON_LIBRARY
  owtrayicon
  CACHE INTERNAL "owtrayicon library"
)

# owtrayicon lib and dependencies
set(OWTRAYICON_LINK_LIBRARIES
  ${OWTRAYICON_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
)

qt4_wrap_cpp(owtrayicon_SRCS
  trayicon.h
)

set(owtrayicon_SRCS
  ${owtrayicon_SRCS}
  trayicon.cpp
)

if (WIN32)
  set(owtrayicon_SRCS ${owtrayicon_SRCS}
    trayicon_win.cpp
  )
endif (WIN32)

if (UNIX AND NOT WIN32)
  if (APPLE)
    set(owtrayicon_SRCS ${owtrayicon_SRCS}
      trayicon_mac.cpp
    )
  else (APPLE)
    set(owtrayicon_SRCS ${owtrayicon_SRCS}
      trayicon_x11.cpp
    )
  endif (APPLE)
endif (UNIX AND NOT WIN32)

qt4_automoc(${owtrayicon_SRCS})

include_directories(
  ${OWTRAYICON_INCLUDE_DIRS}
)

add_library(${OWTRAYICON_LIBRARY} STATIC ${owtrayicon_SRCS})

target_link_libraries(${OWTRAYICON_LINK_LIBRARIES})

