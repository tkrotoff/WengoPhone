project(owutil)

if (UNIX AND NOT WIN32 AND NOT APPLE)
  find_package(UUID REQUIRED)
endif (UNIX AND NOT WIN32 AND NOT APPLE)

set(OWUTIL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWCUTIL_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  CACHE INTERNAL "owutil include directories"
)

set(OWUTIL_LIBRARY
  owutil
  CACHE INTERNAL "owutil library"
)

set(OWUTIL_LINK_LIBRARIES
  ${OWUTIL_LIBRARY}
  ${OWCUTIL_LIBRARY}
  ${BOOST_SIGNALS_LIBRARY}
)

set(owutil_SRCS
  src/exception/Exception.cpp
  src/Base64.cpp
  src/Date.cpp
  src/File.cpp
  src/Logger.cpp
  src/OWPicture.cpp
  src/Path.cpp
  src/String.cpp
  src/StringList.cpp
  src/Time.cpp
  src/Uuid.cpp
  src/WebBrowser.cpp
)

if (WIN32)
  set(owutil_SRCS ${owutil_SRCS}
    src/win/UuidWin.cpp
  )
endif (WIN32)


if (UNIX AND NOT WIN32)
  if (APPLE)
    set(owutil_SRCS ${owutil_SRCS}
      src/mac/UuidMac.cpp
    )
  else (APPLE)
    include_directories(${UUID_INCLUDE_DIRS})
    set(owutil_SRCS ${owutil_SRCS}
      src/unix/UuidUnix.cpp
    )

    set(OWUTIL_LINK_LIBRARIES
      ${OWUTIL_LINK_LIBRARIES}
      ${UUID_LIBRARIES}
    )
  endif (APPLE)
endif (UNIX AND NOT WIN32)

include_directories(
  ${OWUTIL_INCLUDE_DIRS}
)

add_library(${OWUTIL_LIBRARY} STATIC ${owutil_SRCS})

target_link_libraries(${OWUTIL_LINK_LIBRARIES})

if (APPLE)
    macro_add_link_flags(owutil
      "-framework CoreFoundation"
    )
endif (APPLE)

