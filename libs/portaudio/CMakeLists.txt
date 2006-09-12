project(portaudio)

# needed include directories to build portaudio
# saves the variable in internal cache for later use
set(PORTAUDIO_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/pa_common
  CACHE INTERNAL "portaudio include directories"
)

# portaudio lib and dependencies
# saves the variable in internal cache for later use
set(PORTAUDIO_LIBRARIES
  portaudio
  CACHE INTERNAL "portaudio libraries"
)

include_directories(
  ${PORTAUDIO_INCLUDE_DIRS}
)

set(portaudio_SRCS
  pa_common/pa_allocation.c
  pa_common/pa_converters.c
  pa_common/pa_cpuload.c
  pa_common/pa_dither.c
  pa_common/pa_front.c
  pa_common/pa_process.c
  pa_common/pa_skeleton.c
  pa_common/pa_stream.c
  pa_common/pa_trace.c
  pa_win/pa_win_util.c
  pa_win/pa_win_hostapis.c
  pa_win/pa_x86_plain_converters.c
)

if (WITH_PORTAUDIO_DSOUND)
  set(portaudio_SRCS
    ${portaudio_SRCS}
    pa_win_ds/dsound_wrapper.c
    pa_win_ds/pa_win_ds.c
  )
else (WITH_PORTAUDIO_DSOUND)
  set(portaudio_SRCS
    ${portaudio_SRCS}
    pa_win_wmme/pa_win_wmme.c
  )
endif (WITH_PORTAUDIO_DSOUND)

add_library(portaudio STATIC ${portaudio_SRCS})

target_link_libraries(${PORTAUDIO_LIBRARIES})

set_source_files_properties(
  PROPERTIES
    COMPILE_FLAGS
      -DPA_LITTLE_ENDIAN
      -DPA_NO_ASIO
      -DPA_NO_WDMKS
)

