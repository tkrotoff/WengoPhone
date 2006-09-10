# - Try to find FFMPEG
# Once done this will define
#
#  FFMPEG_FOUND - system has FFMPEG
#  FFMPEG_INCLUDE_DIRS - the FFMPEG include directory
#  FFMPEG_LIBRARIES - Link these to use FFMPEG
#  FFMPEG_DEFINITIONS - Compiler switches required for using FFMPEG
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)
  # in cache already
  set(FFMPEG_FOUND TRUE)
else (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)
  find_path(FFMPEG_INCLUDE_DIR
    NAMES
      avcodec.h
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/ffmpeg/include
    PATH_SUFFIXES
      ffmpeg
  )

  find_library(AVUTIL-49_LIBRARY
    NAMES
      avutil-49
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/ffmpeg/binary-lib/msvc
  )
  find_library(AVCODEC-51_LIBRARY
    NAMES
      avcodec-51
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/ffmpeg/binary-lib/msvc
  )
  find_library(AVFORMAT-50_LIBRARY
    NAMES
      avformat-50
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/ffmpeg/binary-lib/msvc
  )

  set(FFMPEG_INCLUDE_DIRS
    ${FFMPEG_INCLUDE_DIR}
  )
  set(FFMPEG_LIBRARIES
    ${AVUTIL-49_LIBRARY}
    ${AVCODEC-51_LIBRARY}
    ${AVFORMAT-50_LIBRARY}
)

  if (FFMPEG_INCLUDE_DIRS AND FFMPEG_LIBRARIES)
     set(FFMPEG_FOUND TRUE)
  endif (FFMPEG_INCLUDE_DIRS AND FFMPEG_LIBRARIES)

  if (FFMPEG_FOUND)
    if (NOT FFMPEG_FIND_QUIETLY)
      message(STATUS "Found FFMPEG: ${FFMPEG_LIBRARIES}")
    endif (NOT FFMPEG_FIND_QUIETLY)
  else (FFMPEG_FOUND)
    if (FFMPEG_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find FFMPEG")
    endif (FFMPEG_FIND_REQUIRED)
  endif (FFMPEG_FOUND)

  # show the FFMPEG_INCLUDE_DIRS and FFMPEG_LIBRARIES variables only in the advanced view
  mark_as_advanced(FFMPEG_INCLUDE_DIRS FFMPEG_LIBRARIES)

endif (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)

