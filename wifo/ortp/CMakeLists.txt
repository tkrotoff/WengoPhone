project(ortp)

# needed include directories to build ortp
# saves the variable in internal cache for later use
set(ORTP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  CACHE INTERNAL "ortp include directories"
)

set(ORTP_LIBRARY
  ortp
  CACHE INTERNAL "ortp library"
)

# ortp lib and dependencies
set(ORTP_LINK_LIBRARIES
  ${ORTP_LIBRARY}
)

set(ORTP_DEFINITIONS
  -DRTP_MAX_SIZE=8192
)

if (WIN32)
  set(ORTP_DEFINITIONS
    ${ORTP_DEFINITIONS}
    -DWIN32
  )

  set(ORTP_LINK_LIBRARIES
    ${ORTP_LINK_LIBRARIES}
    ws2_32
    winmm
  )
endif (WIN32)

if (UNIX AND NOT WIN32)
  set(ORTP_INCLUDE_DIRS
    ${ORTP_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    CACHE INTERNAL "ortp include directories"
  )

  set(ORTP_LINK_LIBRARIES
    ${ORTP_LINK_LIBRARIES}
    ${GLIB2_GTHREAD_LIBRARY}
  )
endif (UNIX AND NOT WIN32)

include_directories(
  ${ORTP_INCLUDE_DIRS}
)

add_definitions(
  ${ORTP_DEFINITIONS}
)

set(ortp_SRCS
  src/avprofile.c
  src/ortp.c
  src/ortpdlkm.c
  src/payloadtype.c
  src/port_fct.c
  src/posixtimer.c
  src/rtpmod.c
  src/rtpparse.c
  src/rtpsession.c
  src/rtpsignaltable.c
  src/rtptimer.c
  src/scheduler.c
  src/sessionset.c
  src/str_utils.c
  src/export.c
  src/telephonyevents.c
  src/rtcp.c
)

add_library(${ORTP_LIBRARY} STATIC ${ortp_SRCS})

target_link_libraries(${ORTP_LINK_LIBRARIES})

