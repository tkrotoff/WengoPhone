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
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  include(UsePkgConfig)

  pkgconfig(glib-2.0 _GLIB2IncDir _GLIB2LinkDir _GLIB2LinkFlags _GLIB2Cflags)

  set(GLIB2_DEFINITIONS ${_GLIB2Cflags})

  find_path(GLIBCONFIG_INCLUDE_DIR
    NAMES
      glibconfig.h
    PATHS
      ${_GLIB2IncDir}
      /opt/gnome/lib64/glib-2.0/include
      /opt/gnome/lib/glib-2.0/include
      /opt/lib/glib-2.0/include
      /usr/lib64/glib-2.0/include
      /usr/lib/glib-2.0/include
      /sw/lib/glib-2.0/include
  )

  find_path(GLIB2_INCLUDE_DIR
    NAMES
      glib.h
    PATHS
      ${_GLIB2IncDir}
      /opt/gnome/include/glib-2.0
      /usr/include/glib-2.0
      /usr/local/include/glib-2.0
      /opt/local/include/glib-2.0
      /sw/include/glib-2.0
  )

  find_library(GLIB2_LIBRARY
    NAMES
      glib-2.0
    PATHS
      ${_GLIB2LinkDir}
      /opt/gnome/lib
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  set(GLIB2_INCLUDE_DIRS
    ${GLIB2_INCLUDE_DIR}
    ${GLIBCONFIG_INCLUDE_DIR}
  )
  set(GLIB2_LIBRARIES
    ${GLIB2_LIBRARY}
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

