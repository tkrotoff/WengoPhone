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
      ${CMAKE_SOURCE_DIR}/libs/gaim/include
  )

  find_library(GAIM_LIBRARY
    NAMES
      gaim
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(ICONV_LIBRARY
    NAMES
      iconv
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(INTL_LIBRARY
    NAMES
      intl
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(NSPR4_LIBRARY
    NAMES
      nspr4
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(NSS3_LIBRARY
    NAMES
      nss3
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(NSSCKBI_LIBRARY
    NAMES
      nssckbi
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(PLC4_LIBRARY
    NAMES
      plc4
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(PLDS4_LIBRARY
    NAMES
      plds4
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(SOFT2KN3_LIBRARY
    NAMES
      soft2kn3
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(SSL3_LIBRARY
    NAMES
      ssl3
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(ZLIB1_LIBRARY
    NAMES
      zlib1
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(IDLETRACK_LIBRARY
    NAMES
      idletrack
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(LIBATK-1.0-0_LIBRARY
    NAMES
      libatk-1.0-0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(LIBGDK_PIXBUF-2.0-0_LIBRARY
    NAMES
      libgdk_pixbuf-2.0-0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(LIBGDK-WIN32-2.0-0_LIBRARY
    NAMES
      libgdk-win32-2.0-0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(LIBGOBJECT-2.0-0_LIBRARY
    NAMES
      libgobject-2.0-0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(LIBGTK-WIN32-2.0-0_LIBRARY
    NAMES
      libgtk-win32-2.0-0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(LIBPANGO-1.0-0_LIBRARY
    NAMES
      libpango-1.0-0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )
  find_library(LIBPANGOWIN32-1.0-0_LIBRARY
    NAMES
      libpangowin32-1.0-0
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/gaim/binary-lib/msvc
  )

  set(GAIM_INCLUDE_DIRS
    ${GAIM_INCLUDE_DIR}
  )
  set(GAIM_LIBRARIES
    ${GAIM_LIBRARY}
    ${ICONV_LIBRARY}
    ${INTL_LIBRARY}
    ${NSPR4_LIBRARY}
    ${NSS3_LIBRARY}
    ${NSSCKBI_LIBRARY}
    ${PLC4_LIBRARY}
    ${PLDS4_LIBRARY}
    ${SOFT2KN3_LIBRARY}
    ${SSL3_LIBRARY}
    ${ZLIB1_LIBRARY}
    ${IDLETRACK_LIBRARY}
    ${LIBATK-1.0-0_LIBRARY}
    ${LIBGDK_PIXBUF-2.0-0_LIBRARY}
    ${LIBGDK-WIN32-2.0-0_LIBRARY}
    ${LIBGOBJECT-2.0-0_LIBRARY}
    ${LIBGTK-WIN32-2.0-0_LIBRARY}
    ${LIBPANGO-1.0-0_LIBRARY}
    ${LIBPANGOWIN32-1.0-0_LIBRARY}
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

