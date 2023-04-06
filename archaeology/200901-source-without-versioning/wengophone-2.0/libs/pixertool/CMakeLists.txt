project(pixertool)

if (WIN32)
  find_package(FFMPEGInternal REQUIRED)
else (WIN32)
  find_package(FFMPEG REQUIRED)
endif (WIN32)

# needed include directories to build pixertool
# saves the variable in internal cache for later use
set(PIXERTOOL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWCUTIL_INCLUDE_DIRS}
  ${FFMPEG_INCLUDE_DIRS}
  CACHE INTERNAL "pixertool include directories"
)

set(PIXERTOOL_LIBRARY
  pixertool
  CACHE INTERNAL "pixertool library"
)

# pixertool lib and dependencies
set(PIXERTOOL_LINK_LIBRARIES
  ${PIXERTOOL_LIBRARY}
  ${OWCUTIL_LIBRARY}
  ${AVUTIL_LIBRARY}
  ${AVCODEC_LIBRARY}
)

set(pixertool_SRCS
  src/pixertool.cpp
  src/ffmpeg/ffmpeg-pixertool.c
)

if (MSVC)
  set(PIXERTOOL_LINK_LIBRARIES
    ${PIXERTOOL_LINK_LIBRARIES}
    winmm
    strmiids
    comsupp
  )

  set(pixertool_SRCS
    ${pixertool_SRCS}
    src/directx/directx-pixertool.cpp
  )
endif (MSVC)

if (UNIX AND NOT WIN32)
  if (APPLE)
    set(pixertool_SRCS
      ${pixertool_SRCS}
      src/quicktime/quicktime-pixertool.c
    )
  else (APPLE)
    set(pixertool_SRCS
      ${pixertool_SRCS}
      src/v4l/v4l-pixertool.c
    )
  endif(APPLE)
else (UNIX AND NOT WIN32)
  set(pixertool_SRCS
    ${pixertool_SRCS}
    src/null-pixertool.cpp
  )
endif (UNIX AND NOT WIN32)

include_directories(
  ${PIXERTOOL_INCLUDE_DIRS}
)

add_library(${PIXERTOOL_LIBRARY} STATIC ${pixertool_SRCS})

target_link_libraries(${PIXERTOOL_LINK_LIBRARIES})

