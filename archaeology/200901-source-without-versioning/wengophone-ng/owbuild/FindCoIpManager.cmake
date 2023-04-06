# - Try to find CoIpManager and all its dependencies included in the
# SDK
#
# Once done this will define
#
#  COIPMANAGER_FOUND - system has CoIpManager
#  COIPMANAGER_INCLUDE_DIRS - the CoIpManager include directory
#  COIPMANAGER_LIBRARIES - Link these to use CoIpManager
#  COIPMANAGER_LIBRARY_DIRS - CoIpManager link directories
#  COIPMANAGER_DEFINITIONS - Compiler switches required for using CoIpManager
#
#  Copyright (c) 2007 Wengo
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (COIPMANAGER_LIBRARIES AND COIPMANAGER_INCLUDE_DIRS)
  # in cache already
  set(COIPMANAGER_FOUND TRUE)
else (COIPMANAGER_LIBRARIES AND COIPMANAGER_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  #include(UsePkgConfig)

  #pkgconfig(CoIpManager COIPMANAGER_INCLUDE_DIRS COIPMANAGER_LIBRARY_DIRS _COIPMANAGER_LINK_FLAGS _COIPMANAGER_DEFINITIONS)

  set(COIPMANAGER_SEARCH_DIRS
    #${_COIPMANAGER_LIBRARY_DIRS}
    $ENV{COIPMANAGER_PATH}/lib
    C:/CoIpManager/lib
    C:/CoIpManager-0.1/lib
    /usr/lib/coipmanager
    /usr/lib/coipmanager-0.1
    /usr/local/lib/coipmanager
    /usr/local/lib/coipmanager-0.1
    /opt/local/lib/coipmanager
    /opt/local/lib/coipmanager-0.1
    /Library/Frameworks/CoIpManager.framework/Libraries
  )

  # NO_DEFAULT_PATH prevents coipmanager.framework directory to be used
  # as the library on Mac OS X

  find_library(COIPMANAGER_LIBRARY
    NAMES
      coipmanager
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
      NO_DEFAULT_PATH
  )

  find_library(COIPMANAGER_BASE_LIBRARY
    NAMES
      coipmanager_base
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(COIPMANAGER_THREADED_LIBRARY
    NAMES
      coipmanager_threaded
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(NETWORKDISCOVERY_LIBRARY
    NAMES
      networkdiscovery
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWCUTIL_LIBRARY
    NAMES
      owcutil
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWUTIL_LIBRARY
    NAMES
      owutil
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWHTTP_LIBRARY
    NAMES
      owhttp
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWMEMORYDUMP_LIBRARY
    NAMES
      owmemorydump
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWSERIALIZATION_LIBRARY
    NAMES
      owserialization
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWSHLIBLOADER_LIBRARY
    NAMES
      owshlibloader
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWSERIALIZER_LIBRARY
    NAMES
      owserializer
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWSYSTEM_LIBRARY
    NAMES
      owsystem
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(OWSETTINGS_LIBRARY
    NAMES
      owsettings
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(SIPWRAPPER_LIBRARY
    NAMES
      sipwrapper
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(PHAPIWRAPPER_LIBRARY
    NAMES
      phapiwrapper
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(SSOREQUEST_LIBRARY
    NAMES
      ssorequest
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(WEBSERVICE_LIBRARY
    NAMES
      webservice
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(PIXERTOOL_LIBRARY
    NAMES
      pixertool
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(WEBCAM_LIBRARY
    NAMES
      webcam
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(QTEVENT_LIBRARY
    NAMES
      qtevent
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(QTCOREUTIL_LIBRARY
    NAMES
      qtcoreutil
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  find_library(SOUND_LIBRARY
    NAMES
      sound
    PATHS
      ${COIPMANAGER_SEARCH_DIRS}
  )

  set(COIPMANAGER_LIBRARIES
    ${COIPMANAGER_LIBRARY}
    ${COIPMANAGER_BASE_LIBRARY}
    ${COIPMANAGER_THREADED_LIBRARY}
    ${NETWORKDISCOVERY_LIBRARY}
    ${OWCUTIL_LIBRARY}
    ${OWUTIL_LIBRARY}
    ${OWHTTP_LIBRARY}
    ${OWMEMORYDUMP_LIBRARY}
    ${OWSERIALIZATION_LIBRARY}
    ${OWSHLIBLOADER_LIBRARY}
    ${OWSERIALIZER_LIBRARY}
    ${OWSYSTEM_LIBRARY}
    ${OWSETTINGS_LIBRARY}
    ${SIPWRAPPER_LIBRARY}
    ${PHAPIWRAPPER_LIBRARY}
    ${SSOREQUEST_LIBRARY}
    ${WEBSERVICE_LIBRARY}
    ${PIXERTOOL_LIBRARY}
    ${WEBCAM_LIBRARY}
    ${QTEVENT_LIBRARY}
    ${QTCOREUTIL_LIBRARY}
    ${SOUND_LIBRARY}
  )

  find_path(COIPMANAGER_INCLUDE_DIRS
    NAMES
      coipmanager/CoIpManager.h
    PATHS
      $ENV{COIPMANAGER_PATH}/include
      C:/CoIpManager/include
      C:/CoIpManager-0.1/include
      /usr/include/coipmanager
      /usr/include/coipmanager-0.1
      /usr/local/include/coipmanager
      /usr/local/include/coipmanager-0.1
      /opt/local/include/coipmanager
      /opt/local/include/coipmanager-0.1
      /Library/Frameworks/CoIpManager.framework/Headers
  )

if (APPLE)
  # Hack to fix a CMake bug (see http://www.cmake.org/Bug/bug.php?op=show&bugid=5139&pos=0)
  set(COIPMANAGER_INCLUDE_DIRS /Library/Frameworks/CoIpManager.framework/Headers)
endif (APPLE)

  # CoIpManager used as a set of dll (shared libraries)
  set(COIPMANAGER_DEFINITIONS
    -DDLL_SUPPORT
  )

  if (COIPMANAGER_INCLUDE_DIRS AND COIPMANAGER_LIBRARIES)
    set(COIPMANAGER_FOUND TRUE)
  endif (COIPMANAGER_INCLUDE_DIRS AND COIPMANAGER_LIBRARIES)

  if (COIPMANAGER_FOUND)
    if (NOT CoIpManager_FIND_QUIETLY)
      message(STATUS "Found CoIpManager: ${COIPMANAGER_LIBRARIES}")
    endif (NOT CoIpManager_FIND_QUIETLY)
  else (COIPMANAGER_FOUND)
    if (CoIpManager_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find CoIpManager")
    endif (CoIpManager_FIND_REQUIRED)
  endif (COIPMANAGER_FOUND)

  # show the COIPMANAGER_INCLUDE_DIRS and COIPMANAGER_LIBRARIES variables only in the advanced view
  mark_as_advanced(COIPMANAGER_INCLUDE_DIRS COIPMANAGER_LIBRARIES)

endif (COIPMANAGER_LIBRARIES AND COIPMANAGER_INCLUDE_DIRS)
