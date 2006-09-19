project(ortp)

# needed include directories to build ortp
# saves the variable in internal cache for later use
set(ORTP_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${GLIB2_INCLUDE_DIRS}
  CACHE INTERNAL "ortp include directories"
)

# ortp lib and dependencies
# saves the variable in internal cache for later use
set(ORTP_LIBRARIES
  ortp
  ${GLIB2_LIBRARIES}
  CACHE INTERNAL "ortp libraries"
)

set(ORTP_DEFINITIONS
  -DRTP_MAX_SIZE=8192
)

if (WIN32)
  set(ORTP_DEFINITIONS
    ${ORTP_DEFINITIONS}
    -DWIN32
  )

  set(ORTP_LIBRARIES
    ${ORTP_LIBRARIES}
    ws2_32
    winmm
    CACHE INTERNAL "ortp libraries"
  )
endif (WIN32)

if (UNIX AND NOT WIN32)
  set(ORTP_INCLUDE_DIRS
    ${ORTP_INCLUDE_DIRS}
    ${GTK2_INCLUDE_DIRS}
    CACHE INTERNAL "ortp include directories"
  )

  set(ORTP_LIBRARIES
    ${ORTP_LIBRARIES}
    ${GTK2_GTHREAD_LIBRARY}
    CACHE INTERNAL "ortp libraries"
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

add_library(ortp STATIC ${ortp_SRCS})

target_link_libraries(${ORTP_LIBRARIES})

