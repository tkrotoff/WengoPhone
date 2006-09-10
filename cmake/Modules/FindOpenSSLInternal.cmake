# - Try to find OpenSSL
# Once done this will define
#
#  OPENSSL_FOUND - system has OpenSSL
#  OPENSSL_INCLUDE_DIRS - the OpenSSL include directory
#  OPENSSL_LIBRARIES - Link these to use OpenSSL
#  OPENSSL_DEFINITIONS - Compiler switches required for using OpenSSL
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (OPENSSL_LIBRARIES AND OPENSSL_INCLUDE_DIRS)
  # in cache already
  set(OPENSSL_FOUND TRUE)
else (OPENSSL_LIBRARIES AND OPENSSL_INCLUDE_DIRS)
  find_path(OPENSSL_INCLUDE_DIR
    NAMES
      openssl/ssl.h
    PATHS
      ${CMAKE_SOURCE_DIR}/libs/3rdparty/openssl/include
  )

  if (MSVC)
    find_library(LIBEAY32_LIBRARY
      NAMES
        libeay32
      PATHS
        ${CMAKE_SOURCE_DIR}/libs/3rdparty/openssl/binary-lib/msvc
    )

    find_library(OPENSSL_LIBRARY
      NAMES
        openssl
      PATHS
        ${CMAKE_SOURCE_DIR}/libs/3rdparty/openssl/binary-lib/msvc
    )

    find_library(SSLEAY32_LIBRARY
      NAMES
        ssleay32
      PATHS
        ${CMAKE_SOURCE_DIR}/libs/3rdparty/openssl/binary-lib/msvc
    )
  endif (MSVC)

  if (MINGW)
    find_library(OPENSSL_LIBRARY
      NAMES
        openssl
        crypto
        ssl
      PATHS
        ${CMAKE_SOURCE_DIR}/libs/3rdparty/openssl/binary-lib/mingw
    )
  endif (MINGW)

  set(OPENSSL_INCLUDE_DIRS
    ${OPENSSL_INCLUDE_DIR}
  )
  set(OPENSSL_LIBRARIES
    ${LIBEAY32_LIBRARY}
    ${OPENSSL_LIBRARY}
    ${SSLEAY32_LIBRARY}
)

  if (OPENSSL_INCLUDE_DIRS AND OPENSSL_LIBRARIES)
     set(OPENSSL_FOUND TRUE)
  endif (OPENSSL_INCLUDE_DIRS AND OPENSSL_LIBRARIES)

  if (OPENSSL_FOUND)
    if (NOT OpenSSL_FIND_QUIETLY)
      message(STATUS "Found OpenSSL: ${OPENSSL_LIBRARIES}")
    endif (NOT OpenSSL_FIND_QUIETLY)
  else (OPENSSL_FOUND)
    if (OpenSSL_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find OpenSSL")
    endif (OpenSSL_FIND_REQUIRED)
  endif (OPENSSL_FOUND)

  # show the OPENSSL_INCLUDE_DIRS and OPENSSL_LIBRARIES variables only in the advanced view
  mark_as_advanced(OPENSSL_INCLUDE_DIRS OPENSSL_LIBRARIES)

endif (OPENSSL_LIBRARIES AND OPENSSL_INCLUDE_DIRS)

