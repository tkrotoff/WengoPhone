# - Try to find GLIB2
# Once done this will define
#
#  GLIB2_FOUND - system has GLIB2
#  GLIB2_INCLUDE_DIRS - the GLIB2 include directory
#  GLIB2_LIBRARIES - Link these to use GLIB2
#  GLIB2_DEFINITIONS - Compiler switches required for using GLIB2
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
  # in cache already
  set(GLIB2_FOUND TRUE)
else (GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
  find_path(GLIB2_INCLUDE_DIR
    NAMES
      glibconfig.h
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/glib/include
  )

  find_library(GLIB2_LIBRARY
    NAMES
      glib-2.0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/glib/binary-lib/msvc
  )
  find_library(GMODULE2_LIBRARY
    NAMES
      gmodule-2.0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/glib/binary-lib/msvc
  )
  find_library(GOBJECT2_LIBRARY
    NAMES
      gobject-2.0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/glib/binary-lib/msvc
  )
  find_library(GTHREAD2_LIBRARY
    NAMES
      gthread-2.0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/glib/binary-lib/msvc
  )

  set(GLIB2_INCLUDE_DIRS
    ${GLIB2_INCLUDE_DIR}
  )
  set(GLIB2_LIBRARIES
    ${GLIB2_LIBRARY}
    ${GMODULE2_LIBRARY}
    ${GTHREAD2_LIBRARY}
)

  if (GLIB2_INCLUDE_DIRS AND GLIB2_LIBRARIES)
     set(GLIB2_FOUND TRUE)
  endif (GLIB2_INCLUDE_DIRS AND GLIB2_LIBRARIES)

  if (GLIB2_FOUND)
    if (NOT GLIB2_FIND_QUIETLY)
      message(STATUS "Found GLIB2: ${GLIB2_LIBRARIES}")
    endif (NOT GLIB2_FIND_QUIETLY)
  else (GLIB2_FOUND)
    if (GLIB2_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find GLIB2")
    endif (GLIB2_FIND_REQUIRED)
  endif (GLIB2_FOUND)

  # show the GLIB2_INCLUDE_DIRS and GLIB2_LIBRARIES variables only in the advanced view
  mark_as_advanced(GLIB2_INCLUDE_DIRS GLIB2_LIBRARIES)

endif (GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
