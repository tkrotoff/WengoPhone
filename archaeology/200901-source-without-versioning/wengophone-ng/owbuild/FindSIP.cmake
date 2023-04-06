# - Try to find SIP (sip)
# Once done this will define
#
#  SIP_FOUND - true if the command line was found
#  SIP_EXECUTABLE - path to sip command line
#  SIP_INCLUDE_DIRS - SIP include directories
#
#  Copyright (c) 2007 Wengo
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (SIP_EXECUTABLE AND SIP_INCLUDE_DIRS)
  # in cache already
  set(SIP_FOUND TRUE)
else (SIP_EXECUTABLE AND SIP_INCLUDE_DIRS)

  find_program(SIP_EXECUTABLE
    NAMES
      sip
    PATHS
      /System/Library/Frameworks/Python.framework/Versions/2.3/bin/
      /usr/bin
      c:/Python25
      c:/Python24
      "c:/Program Files/Python24"
      "c:/Program Files/Python25"
  )

  find_path(SIP_INCLUDE_DIR
    NAMES
      sip.h
    PATHS
      /System/Library/Frameworks/Python.framework/Headers
      /usr/include
      c:/Python25/include
      c:/Python24/include
      "c:/Program Files/Python24/include"
      "c:/Program Files/Python25/include"
    PATH_SUFFIXES
      python2.5
      python2.4
  )

  set(SIP_INCLUDE_DIRS ${SIP_INCLUDE_DIR})

  if (SIP_EXECUTABLE AND SIP_INCLUDE_DIRS)
    set(SIP_FOUND TRUE)
  endif (SIP_EXECUTABLE AND SIP_INCLUDE_DIRS)

  if (SIP_FOUND)
    if (NOT SIP_FIND_QUIETLY)
      message(STATUS "Found SIP (sip): ${SIP_EXECUTABLE}, ${SIP_INCLUDE_DIR}")
    endif (NOT SIP_FIND_QUIETLY)
  else (SIP_FOUND)
    if (SIP_FIND_REQUIRED)
      if (NOT SIP_EXECUTABLE)
        message(ERROR "Could not find SIP executable")
      endif (NOT SIP_EXECUTABLE)
      if (NOT SIP_INCLUDE_DIRS)
        message(ERROR "Could not find SIP include dirs")
      endif (NOT SIP_INCLUDE_DIRS)
      message(FATAL_ERROR "Could not find SIP (sip)")
    endif (SIP_FIND_REQUIRED)
  endif (SIP_FOUND)

  # show the SIP_EXECUTABLE and SIP_INCLUDE_DIRS variables only in the advanced view
  mark_as_advanced(SIP_EXECUTABLE SIP_INCLUDE_DIRS)

endif (SIP_EXECUTABLE AND SIP_INCLUDE_DIRS)
