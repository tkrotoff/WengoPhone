project(eXosip)

#if (UNIX AND NOT WIN32)
#  find_package(OSIP2 REQUIRED)
#endif (UNIX AND NOT WIN32)

# needed include directories to build eXosip
# saves the variable in internal cache for later use
set(EXOSIP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OSIP2_INCLUDE_DIRS}
  CACHE INTERNAL "eXosip include directories"
)

# eXosip lib and dependencies
# saves the variable in internal cache for later use
set(EXOSIP_LIBRARIES
  eXosip
  ${OSIP2_LIBRARIES}
  CACHE INTERNAL "eXosip libraries"
)

if (WIN32)
  set(EXOSIP_LIBRARIES
    ${EXOSIP_LIBRARIES}
    ws2_32
    winmm
    iphlpapi
    CACHE INTERNAL "eXosip libraries"
  )
endif (WIN32)

set(EXOSIP_DEFINITIONS
  ${OSIP2_DEFINITIONS}
)

include_directories(
  ${EXOSIP_INCLUDE_DIRS}
)

add_definitions(
  ${EXOSIP_DEFINITIONS}
)

set(eXosip_SRCS
  src/eXosip.c
  src/eXutils.c
  src/jauth.c
  src/jcall.c
  src/jcallback.c
  src/jdialog.c
  src/jevents.c
  src/jfreinds.c
  src/jidentity.c
  src/jmsg.c
  src/jnotify.c
  src/jpipe.c
  src/jreg.c
  src/jrequest.c
  src/jresponse.c
  src/jsubscribe.c
  src/jsubscribers.c
  src/misc.c
  src/sdp_offans.c
  src/udp.c
)

add_library(eXosip STATIC ${eXosip_SRCS})

target_link_libraries(${EXOSIP_LIBRARIES})

