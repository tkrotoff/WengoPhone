project(phapi)

subdirs(
  amrnb
  amrwb
  fidlib
  gsm
  ilbc
  speexec
  stun
)

if (MSVC)
  set(PHAPI_OPTIMIZATION_FLAGS
    /Ox
    /Ob2
    /Oi
    /Ot
    /Oy
    /G7
    /GX-
    /GR-
    /arch:SSE
  )
else (MSVC)
  # TODO: we need a flag on other platforms
  # maybe unroll-loops is nice to have ;)
  set(PHAPI_OPTIMIZATION_FLAGS
    -funroll-loops
  )
endif (MSVC)


# needed include directories to build phapi
# saves the variable in internal cache for later use
set(PHAPI_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${EXOSIP_INCLUDE_DIRS}
  ${ORTP_INCLUDE_DIRS}
  ${ILBC_INCLUDE_DIRS}
  ${STUN_INCLUDE_DIRS}
  ${GSM_INCLUDE_DIRS}
  ${OWUTIL_INCLUDE_DIRS}
  ${PHAPI-UTIL_INCLUDE_DIRS}
  ${FIDLIB_INCLUDE_DIRS}
  ${SPEEXEC_INCLUDE_DIRS}
  CACHE INTERNAL "phapi include directories"
)

set(PHAPI_LIBRARY
  phapi
  CACHE INTERNAL "phapi library"
)

# phapi lib and dependencies
# saves the variable in internal cache for later use
set(PHAPI_LIBRARIES
  phapi
  ${EXOSIP_LIBRARIES}
  ${ORTP_LIBRARIES}
  ${ILBC_LIBRARIES}
  ${STUN_LIBRARIES}
  ${GSM_LIBRARIES}
  ${OWUTIL_LIBRARIES}
  ${PHAPI-UTIL_LIBRARIES}
  ${FIDLIB_LIBRARIES}
  ${SPEEXEC_LIBRARIES}
  CACHE INTERNAL "phapi libraries"
)

set(PHAPI_DEFINITIONS
  -DUSE_PCM=1
  -DNEED_ECMUX=1
  -DMINIMIZE_COPYING=1
  -DSCHEDULING_MODE=0
  -DBLOCKING_MODE=0
  -DDO_ECHO_CAN=1
  -DENABLE_AMR_WB=1
  -DENABLE_AMR=1
  -DMY_DLLEXPORTS=1
  -DUSE_WAVEIN_CBK=1
  -DPSEUDO_AEC=0
  -DNO_ECHO__SUPPRESSOR=1
  -DENABLE_ILBC=1
  -DOSIP_MT=1
  -DENABLE_TRACE=0
  -DPH_VIDEO_USELOCK=1
  -DUSE_SPXEC=1
  -DUSE_PLUGINS=1
  -DPH_FORCE_16KHZ=1
  -DSUBVERSION_REVISION=${SVN_REVISION}
  -DWENGOPHONE_UA="\\"phapi/eXosip/0.2.0\\""
)

# PH_FORCE_16KHZ : activate FORCED 16 KHZ SAMPLING
# SUBVERSION_REVISION : grab the svn revision number
# WENGOPHONE_UA : set the wengophone user agent. It should
#                 be later changed to "Wengophone NG"

set(phapi_SRCS
  phcodec.c
  phmbuf.c
  wav.c
  tonegen.c
  phmedia-audio-null.c
  phmedia-audio-file.c
  phapi.c
  phaudiodriver.c
  phmedia.c
  phrecorder.c
  phmedia-audio.c
  aec2.cpp
  phresample.c
  phlog.c
  phplugin.c
  phplugin-sdp.c
)

# phapi plugins
set(phapi_amr_plugin_SRCS
  phcodec-amr-wrapper.c
)

set(phapi_speex_plugin_SRCS
  phcodec-speex.c
)


### phapi options

# plugins
option(WITH_SPEEXWB_REPLACES_AMRWB "" OFF)
option(WITH_SPEEXWB-REPLACES-AMRNB "" OFF)
option(WITH_SPEEXWB-REPLACES-G726WB "" ON)

if (WITH_SPEEXWB-REPLACES-G726WB)
  set(PHAPI_DEFINITIONS
    ${PHAPI_DEFINITIONS}
    -DPH_SPEEXWB_REPLACES_G726WB
  )

  set(PHAPI_AMR_DEFINITIONS
    -DPH_SPEEXWB_REPLACES_G726WB
  )

  set(PHAPI_SPEEXEC_DEFINITONS
    -DPH_SPEEXWB_REPLACES_G726WB
  )
else (WITH_SPEEXWB-REPLACES-G726WB)
  if (WITH_SPEEXWB-REPLACES-AMRNB)
    set(PHAPI_DEFINITIONS
      ${PHAPI_DEFINITIONS}
      -DPH_SPEEXWB_REPLACES_AMRNB
    )

    set(PHAPI_AMR_DEFINITIONS
      -DPH_SPEEXWB_REPLACES_AMRNB
    )

    set(PHAPI_SPEEXEC_DEFINITONS
      -DPH_SPEEXWB_REPLACES_AMRNB
    )
  else (WITH_SPEEXWB-REPLACES-AMRNB)
    if (WITH_SPEEXWB_REPLACES_AMRWB)
      set(PHAPI_DEFINITIONS
        ${PHAPI_DEFINITIONS}
        -DPH_SPEEXWB_REPLACES_AMRWB
      )

      set(PHAPI_AMR_DEFINITIONS
        -DPH_SPEEXWB_REPLACES_AMRWB
      )

      set(PHAPI_SPEEXEC_DEFINITONS
        -D-DPH_SPEEXWB_REPLACES_AMRWB
      )
    endif (WITH_SPEEXWB_REPLACES_AMRWB)
  endif (WITH_SPEEXWB-REPLACES-AMRNB)
endif (WITH_SPEEXWB-REPLACES-G726WB)

# embedded
option(WITH_EMBEDDED_PHAPI "" OFF)
if (WITH_EMBEDDED_PHAPI)
  add_definitions(-DSCHEDULING_MODE)
endif (WITH_EMBEDDED_PHAPI)

option(WITH_SHARED_PHAPI "Build phapi as shared library" ON)

option(WITH_QOS_DEBUG "Build with QOS debug support" OFF)
if (WITH_QOS_DEBUG)
  add_definitons(-DPHAPI_QOS_DEBUG)
endif (WITH_QOS_DEBUG)

option(WITH_HTTPTUNNEL "Build phapi with httptunnel support" ON)
if (WITH_HTTPTUNNEL)
  set(PHAPI_INCLUDE_DIRS
    ${PHAPI_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
    ${HTTPTUNNEL_INCLUDE_DIRS}
    CACHE INTERNAL "phapi include directories"
  )

  set(PHAPI_LIBRARIES
    ${PHAPI_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    httptunnel
  )

  add_definitions(-DUSE_HTTP_TUNNEL)
endif (WITH_HTTPTUNNEL)

if (WITH_VIDEO)
  set(PHAPI_INCLUDE_DIRS
    ${PHAPI_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${WEBCAM_INCLUDE_DIRS}
    CACHE INTERNAL "phapi include directories"
  )

  set(PHAPI_LIBRARIES
    ${PHAPI_LIBRARIES}
    ${AVUTIL_LIBRARY}
    ${AVCODEC_LIBRARY}
    ${WEBCAM_LIBRARIES}
    CACHE INTERNAL "phapi libraries"
  )

  add_definitions(-DPHAPI_VIDEO_SUPPORT)

  set(phapi_SRCS
    ${phapi_SRCS}
    phcodec-h263.c
    phcodec-h264.c
    phcodec-mpeg4.c
    phmedia-video.c
    phmedia-video-control.c
    phmedia-video-bwcontrol.c
    phcodec-avcodec-wrapper.c
  )
endif (WITH_VIDEO)

# system specific phapi options

if (WIN32)
  set(PHAPI_LIBRARIES
    ${PHAPI_LIBRARIES}
      msdirent
      winmm
      ws2_32
      user32
      shell32
      advapi32
      wininet
      iphlpapi
      strmiids
      ole32
      CACHE INTERNAL "phapi libraries"
  )

  set(phapi_SRCS
    ${phapi_SRCS}
    phmedia-winmm.c
  )

  if (MSVC)
    set(PHAPI_DEFINITIONS
      ${PHAPI_DEFINITIONS}
      -DT_MSVC
    )

    set(PHAPI_LIBRARIES
      ${PHAPI_LIBRARIES}
      comsupp
      CACHE INTERNAL "phapi libraries"
    )
  else (MSVC)
    set(PHAPI_LIBRARIES
      ${PHAPI_LIBRARIES}
      gdi32
      CACHE INTERNAL "phapi libraries"
    )
  endif (MSVC)
endif (WIN32)

if (UNIX AND NOT WIN32)
  if (APPLE)
    set(WITH_PHAPI_SHARED OFF)

    set(phapi_SRCS
      ${phapi_SRCS}
      phmedia-coreaudio.c
    )

    macro_add_link_flags(phapi
      "-framework AudioUnit"
      "-framework CoreAudio"
      "-framework AudioToolbox"
      "-framework CoreServices"
    )
  else (APPLE)
    set(PHAPI_INCLUDE_DIRS
      ${PHAPI_INCLUDE_DIRS}
      ${GTK2_INCLUDE_DIRS}
      CACHE INTERNAL "phapi include directories"
    )

    set(PHAPI_LIBRARIES
      ${PHAPI_LIBRARIES}
      ${GTK2_GTHREAD_LIBRARY}
      pthread
      rt
      CACHE INTERNAL "phapi libraries"
    )

    option(WITH_SAMPLERATE "Build phapi with samplerate support" OFF)
    if (WITH_SAMPLERATE)
      set(PHAPI_LIBRARIES
        ${PHAPI_LIBRARIES}
        ${SAMPLERATE_LIBRARIES}
        CACHE INTERNAL "phapi libraries"
      )

      add_definitions(-DPH_USE_RESAMPLE)
    endif (WITH_SAMPLERATE)

    option(WITH_PHAPI_AUDIO_ALSA "Build phapi with alsa support" ON)
    if (WITH_PHAPI_AUDIO_ALSA)
      find_package(Alsa REQUIRED)

      set(PHAPI_LIBRARIES
        ${PHAPI_LIBRARIES}
        ${ASOUND_LIBRARY}
        CACHE INTERNAL "phapi libraries"
      )

      add_definitions(-DENABLE_ALSA)

      set(phapi_SRCS
        ${phapi_SRCS}
        phmedia-alsa.c
      )
    endif (WITH_PHAPI_AUDIO_ALSA)

    option(WITH_PHAPI_AUDIO_ARTS "Build phapi with arts support" OFF)
    if (WITH_PHAPI_AUDIO_ARTS)
      find_package(ARTS REQUIRED)

      include_directories(
        ${ARTS_INCLUDE_DIRS}
      )

      set(PHAPI_LIBRARIES
        ${PHAPI_LIBRARIES}
        ${ARTS_LIBRARY}
        CACHE INTERNAL "phapi libraries"
      )

      add_definitions(-DENABLE_ARTS)
      set(phapi_SRCS
        ${phapi_SRCS}
        phmedia-audio-arts.c
      )
    endif (WITH_PHAPI_AUDIO_ARTS)

    option(WITH_PHAPI_AUDIO_ESD "Build phapi with esd support" OFF)
    if (WITH_PHAPI_AUDIO_ESD)
      find_package(ESD REQUIRED)

      include_directories(
        ${ESD_INCLUDE_DIRS}
      )

      set(PHAPI_LIBRARIES
        ${PHAPI_LIBRARIES}
        ${ESD_LIBRARY}
        CACHE INTERNAL "phapi libraries"
      )
      add_definitions(-DENABLE_ESD)

      set(phapi_SRCS
        ${phapi_SRCS}
        phmedia-audio-esd.c
      )
    endif (WITH_PHAPI_AUDIO_ESD)
  endif (APPLE)
endif (UNIX AND NOT WIN32)

if (NOT APPLE)
  set(PHAPI_LIBRARIES
    ${PHAPI_LIBRARIES}
    ${PORTAUDIO_LIBRARIES}
    CACHE INTERNAL "phapi libraries"
  )

  add_definitions(-DENABLE_PORTAUDIO)

  set(phapi_SRCS
    ${phapi_SRCS}
    phmedia-portaudio.c
  )
endif (NOT APPLE)

include_directories(
  ${PHAPI_INCLUDE_DIRS}
)

add_definitions(
  ${PHAPI_DEFINITIONS}
)

# phapi
if (WITH_SHARED_PHAPI)
  add_definitions(-DBUILD_PHAPI_DLL)
  add_library(phapi SHARED ${phapi_SRCS})
else (WITH_SHARED_PHAPI)
  add_library(phapi STATIC ${phapi_SRCS})
endif (WITH_SHARED_PHAPI)

target_link_libraries(${PHAPI_LIBRARIES})

set_target_properties(
  phapi
  PROPERTIES
    VERSION
      0.2.0
    SOVERSION
      0
)

install(
  TARGETS
    phapi
  LIBRARY
  DESTINATION
    ${LIB_INSTALL_DIR}
)

### phapi plugins

# amr
macro_add_plugin(phamrplugin ${phapi_amr_plugin_SRCS})
macro_add_compile_flags(phamrplugin ${PHAPI_OPTIMIZATION_FLAGS})
target_link_libraries(phamrplugin amrwb amrnb)

# speex
find_package(Speex REQUIRED)
include_directories(${SPEEX_INCLUDE_DIRS})
macro_add_plugin(phspeexplugin ${phapi_speex_plugin_SRCS})
macro_add_compile_flags(phspeexplugin ${PHAPI_OPTIMIZATION_FLAGS})
target_link_libraries(phspeexplugin ${SPEEX_LIBRARIES})

