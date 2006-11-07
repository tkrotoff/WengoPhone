# - Try to find GAIM
# Once done this will define
#
#  GAIM_FOUND - system has GAIM
#  GAIM_INCLUDE_DIRS - the GAIM include directory
#  GAIM_LIBRARIES - Link these to use GAIM
#  GAIM_DEFINITIONS - Compiler switches required for using GAIM
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (GAIM_LIBRARIES AND GAIM_INCLUDE_DIRS)
  # in cache already
  set(GAIM_FOUND TRUE)
else (GAIM_LIBRARIES AND GAIM_INCLUDE_DIRS)
  find_path(GAIM_INCLUDE_DIR
    NAMES
      gaim/core.h
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/gaim/include
  )

  find_path(GAIM_PLUGIN_INCLUDE_DIR
    NAMES
      plugin.h
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/gaim/src/libgaim
  )

  find_library(GAIM_LIBRARIES
    NAMES
      gaim
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/gaim/binary-lib/msvc
  )

  set(GAIM_INCLUDE_DIRS
    ${GAIM_INCLUDE_DIR}
    ${GAIM_PLUGIN_INCLUDE_DIR}
  )

  if (GAIM_INCLUDE_DIRS AND GAIM_LIBRARIES)
     set(GAIM_FOUND TRUE)
  endif (GAIM_INCLUDE_DIRS AND GAIM_LIBRARIES)

  if (GAIM_FOUND)
    if (NOT GAIM_FIND_QUIETLY)
      message(STATUS "Found GAIM: ${GAIM_LIBRARIES}")
    endif (NOT GAIM_FIND_QUIETLY)
  else (GAIM_FOUND)
    if (GAIM_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find GAIM")
    endif (GAIM_FIND_REQUIRED)
  endif (GAIM_FOUND)

  # show the GAIM_INCLUDE_DIRS and GAIM_LIBRARIES variables only in the advanced view
  mark_as_advanced(GAIM_INCLUDE_DIRS GAIM_LIBRARIES)

endif (GAIM_LIBRARIES AND GAIM_INCLUDE_DIRS)

