project(osip2)

# needed include directories to build osip
# saves the variable in internal cache for later use
set(OSIP2_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/osipparser2
  ${CMAKE_CURRENT_SOURCE_DIR}/src/osip2
  ${CMAKE_CURRENT_SOURCE_DIR}/src/osipparser2
  CACHE INTERNAL "osip include directories"
)

# osip lib and dependencies
# saves the variable in internal cache for later use
set(OSIP2_LIBRARIES
  osip2
  CACHE INTERNAL "osip libraries"
)

set(OSIP2_DEFINITIONS
  -DOSIP_MT
  -DENABLE_TRACE
  CACHE INTERNAL "osip definitions"
)

if (WIN32)
  set(OSIP2_LIBRARIES
    ${OSIP_LIBRARIES}
    advapi32
  )

  set(OSIP2_DEFINITIONS
    ${OSIP_DEFINITIONS}
    -DWIN32
  )
endif (WIN32)

if (UNIX AND NOT WIN32)
  if (APPLE)
    set(OSIP2_DEFINITIONS
      ${OSIP2_DEFINITIONS}
      -DHAVE_PTHREAD
      -DHAVE_SYS_SEM_H
    )
  else (APPLE)
    set(OSIP2_DEFINITIONS
      ${OSIP2_DEFINITIONS}
      -DHAVE_PTHREAD
      -DHAVE_SEMAPHORE_H
    )
  endif (APPLE)
endif (UNIX AND NOT WIN32)

include_directories(
  ${OSIP2_INCLUDE_DIRS}
)

add_definitions(
  ${OSIP2_DEFINITIONS}
)

set(osip2_SRCS
  src/osipparser2/osip_accept.c
  src/osipparser2/osip_accept_encoding.c
  src/osipparser2/osip_accept_language.c
  src/osipparser2/osip_alert_info.c
  src/osipparser2/osip_allow.c
  src/osipparser2/osip_authorization.c
  src/osipparser2/osip_body.c
  src/osipparser2/osip_call_id.c
  src/osipparser2/osip_call_info.c
  src/osipparser2/osip_contact.c
  src/osipparser2/osip_content_disposition.c
  src/osipparser2/osip_content_encoding.c
  src/osipparser2/osip_content_length.c
  src/osipparser2/osip_content_type.c
  src/osipparser2/osip_cseq.c
  src/osipparser2/osip_error_info.c
  src/osipparser2/osip_from.c
  src/osipparser2/osip_header.c
  src/osipparser2/osip_list.c
  src/osipparser2/osip_md5c.c
  src/osipparser2/osip_message.c
  src/osipparser2/osip_message_parse.c
  src/osipparser2/osip_message_to_str.c
  src/osipparser2/osip_mime_version.c
  src/osipparser2/osip_parser_cfg.c
  src/osipparser2/osip_port.c
  src/osipparser2/osip_proxy_authenticate.c
  src/osipparser2/osip_proxy_authorization.c
  src/osipparser2/osip_record_route.c
  src/osipparser2/osip_rfc3264.c
  src/osipparser2/osip_route.c
  src/osipparser2/osip_to.c
  src/osipparser2/osip_uri.c
  src/osipparser2/osip_via.c
  src/osipparser2/osip_www_authenticate.c
  src/osipparser2/sdp_accessor.c
  src/osipparser2/sdp_message.c
  src/osipparser2/osip_replaces.c

  src/osip2/fsm_misc.c
  src/osip2/ict.c
  src/osip2/ict_fsm.c
  src/osip2/ist.c
  src/osip2/ist_fsm.c
  src/osip2/nict.c
  src/osip2/nict_fsm.c
  src/osip2/nist.c
  src/osip2/nist_fsm.c
  src/osip2/osip.c
  src/osip2/osip_dialog.c
  src/osip2/osip_event.c
  src/osip2/osip_negotiation.c
  src/osip2/osip_time.c
  src/osip2/osip_transaction.c
  src/osip2/port_condv.c
  src/osip2/port_fifo.c
  src/osip2/port_sema.c
  src/osip2/port_thread.c
)


add_library(osip2 STATIC ${osip2_SRCS})

target_link_libraries(${OSIP2_LIBRARIES})

