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

set(EXOSIP_LIBRARY
  eXosip
  CACHE INTERNAL "eXosip library"
)

# eXosip lib and dependencies
set(EXOSIP_LINK_LIBRARIES
  ${EXOSIP_LIBRARY}
  ${OSIP2_LIBRARY}
)

if (WIN32)
  set(EXOSIP_LINK_LIBRARIES
    ${EXOSIP_LINK_LIBRARIES}
    ws2_32
    winmm
    iphlpapi
  )
endif (WIN32)

set(EXOSIP_DEFINITIONS
  ${OSIP2_DEFINITIONS}
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

include_directories(
  ${EXOSIP_INCLUDE_DIRS}
)

add_definitions(
  ${EXOSIP_DEFINITIONS}
)

add_library(${EXOSIP_LIBRARY} STATIC ${eXosip_SRCS})

target_link_libraries(${EXOSIP_LINK_LIBRARIES})

