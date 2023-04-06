project(owwebcam)

# needed include directories to build owwebcam
# saves the variable in internal cache for later use
set(OWWEBCAM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  ${PIXERTOOL_INCLUDE_DIRS}
  CACHE INTERNAL "owwebcam include directories"
)

if (UNIX AND NOT WIN32 OR APPLE)
  set(OWWEBCAM_INCLUDE_DIRS
    ${OWWEBCAM_INCLUDE_DIRS}
    ${OWTHREAD_INCLUDE_DIRS}
    CACHE INTERNAL "owwebcam include directories"
  )
endif (UNIX AND NOT WIN32 OR APPLE)

set(OWWEBCAM_LIBRARY
  owwebcam
  CACHE INTERNAL "owwebcam library"
)

# owwebcam lib and dependencies
set(OWWEBCAM_LINK_LIBRARIES
  ${OWWEBCAM_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${PIXERTOOL_LIBRARY}
)

if(UNIX AND NOT WIN32 OR APPLE)
  set(OWWEBCAM_LINK_LIBRARIES
    ${OWWEBCAM_LINK_LIBRARIES}
    ${OWTHREAD_LIBRARY}
  )
endif(UNIX AND NOT WIN32 OR APPLE)

set(owwebcam_SRCS
  src/WebcamDriver.cpp
  src/webcam.cpp
)

if (MSVC)
  set(OWWEBCAM_LINK_LIBRARIES
    ${OWWEBCAM_LINK_LIBRARIES}
    winmm
    strmiids
    comsupp
  )

  set(owwebcam_SRCS
    ${owwebcam_SRCS}
    src/directx/DirectXWebcamDriver.cpp
    src/directx/DirectXWebcamUtils.cpp
  )
endif (MSVC)

if (UNIX AND NOT WIN32)
  if (APPLE)
    set(OWWEBCAM_INCLUDE_DIRS
      ${OWWEBCAM_INCLUDE_DIRS}
      /System/Library/Frameworks/Quicktime.framework/Headers
      CACHE INTERNAL "owwebcam include directories"
    )

    set(owwebcam_SRCS
      ${owwebcam_SRCS}
      src/quicktime/QuicktimeWebcamDriver.cpp
    )
  else (APPLE)
    set(owwebcam_SRCS
      ${owwebcam_SRCS}
      src/v4l/V4LWebcamDriver.cpp
    )
  endif (APPLE)
endif (UNIX AND NOT WIN32)

include_directories(
  ${OWWEBCAM_INCLUDE_DIRS}
)

add_library(${OWWEBCAM_LIBRARY} STATIC ${owwebcam_SRCS})

target_link_libraries(${OWWEBCAM_LINK_LIBRARIES})

