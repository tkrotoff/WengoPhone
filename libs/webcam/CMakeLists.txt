project(webcam)

# needed include directories to build webcam
# saves the variable in internal cache for later use
set(WEBCAM_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  ${PIXERTOOL_INCLUDE_DIRS}
  CACHE INTERNAL "webcam include directories"
)

if (UNIX AND NOT WIN32 OR APPLE)
  set(WEBCAM_INCLUDE_DIRS
    ${WEBCAM_INCLUDE_DIRS}
    ${OWTHREAD_INCLUDE_DIRS}
    CACHE INTERNAL "webcam include directories"
  )
endif (UNIX AND NOT WIN32 OR APPLE)

# webcam lib and dependencies
# saves the variable in internal cache for later use
set(WEBCAM_LIBRARIES
  webcam
  ${OWUTIL_LIBRARIES}
  ${PIXERTOOL_LIBRARIES}
  CACHE INTERNAL "webcam libraries"
)

if(UNIX AND NOT WIN32 OR APPLE)
  set(WEBCAM_LIBRARIES
    ${WEBCAM_LIBRARIES}
    ${OWTHREAD_LIBRARIES}
    CACHE INTERNAL "webcam libraries"
  )
endif(UNIX AND NOT WIN32 OR APPLE)

set(webcam_SRCS
  src/WebcamDriver.cpp
  src/webcam.cpp
  src/null/NullWebcamDriver.cpp
)

if (MSVC)
  set(WEBCAM_LIBRARIES
    ${WEBCAM_LIBRARIES}
    winmm
    strmiids
    comsupp
  )

  set(webcam_SRCS
    ${webcam_SRCS}
    src/directx/DirectXWebcamDriver.cpp
    src/directx/DirectXWebcamUtils.cpp
  )
endif (MSVC)

if (UNIX AND NOT WIN32)
  if (APPLE)
    set(WEBCAM_INCLUDE_DIRS
      ${WEBCAM_INCLUDE_DIRS}
      CACHE INTERNAL "webcam include directories"
    )

    set(webcam_SRCS
      ${webcam_SRCS}
      src/quicktime/QuicktimeWebcamDriver.cpp
    )
  else (APPLE)
    set(webcam_SRCS
      ${webcam_SRCS}
      src/v4l/V4LWebcamDriver.cpp
    )
  endif (APPLE)
endif (UNIX AND NOT WIN32)

include_directories(
  ${WEBCAM_INCLUDE_DIRS}
)

add_library(webcam STATIC ${webcam_SRCS})

target_link_libraries(${WEBCAM_LIBRARIES})

