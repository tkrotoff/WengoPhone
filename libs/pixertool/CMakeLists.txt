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

# pixertool lib and dependencies
# saves the variable in internal cache for later use
set(PIXERTOOL_LIBRARIES
  pixertool
  ${OWCUTIL_LIBRARIES}
  ${AVUTIL_LIBRARY}
  ${AVCODEC_LIBRARY}
  CACHE INTERNAL "pixertool libraries"
)

include_directories(
  ${PIXERTOOL_INCLUDE_DIRS}
)

set(pixertool_SRCS
  src/pixertool.cpp
  src/ffmpeg/ffmpeg-pixertool.c
)

# TODO: find_lib needed?
if (MSVC)
  set(PIXERTOOL_LIBRARIES ${PIXERTOOL_LIBRARIES}
    winmm
    strmiids
    comsupp
  )
  set(pixertool_SRC ${pixertool_SRC}
    src/directx/directx-pixertool.cpp
  )
endif (MSVC)

if (NOT WIN32)
  if (APPLE)
    set(pixertool_SRC ${pixertool_SRC}
      src/quicktime/quicktime-pixertool.c
    )
  else (APPLE)
    set(pixertool_SRC ${pixertool_SRC}
      src/v4l/v4l-pixertool.c
    )
  endif(APPLE)
else (NOT WIN32)
  set(pixertool_SRC ${pixertool_SRC}
    src/null-pixertool.cpp
  )
endif (NOT WIN32)

add_library(pixertool STATIC ${pixertool_SRCS})

target_link_libraries(${PIXERTOOL_LIBRARIES})

