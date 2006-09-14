project(owutil)

#Boost dependency
find_package(Boost REQUIRED)

if (UNIX AND NOT WIN32)
  find_package(UUID REQUIRED)
endif (UNIX AND NOT WIN32)

#owcutil dependency
#link_directories(${owcutil_BINARY_DIR})

#includes
set(OWUTIL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWCUTIL_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  CACHE INTERNAL "owutil include directories"
)

set(OWUTIL_LIBRARIES
  owutil
  owcuitl
)

include_directories(
  ${OWUTIL_INCLUDE_DIRS}
  ${OWCUTIL_INCLUDE_DIRS}
)


#sources
set(owutil_SRCS
  src/exception/Exception.cpp
  src/Base64.cpp
  src/Date.cpp
  src/File.cpp
  src/Logger.cpp
  src/Path.cpp
  src/Picture.cpp
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
  endif (APPLE)
endif (UNIX AND NOT WIN32)

add_library(owutil STATIC ${owutil_SRCS})

target_link_libraries(${OWUTIL_LIBRARIES})

if (UNIX AND NOT WIN32)
  target_link_libraries(${UUID_LIBRARIES})
endif (UNIX AND NOT WIN32)
