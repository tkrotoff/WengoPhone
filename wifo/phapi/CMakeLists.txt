option(WITH_SPEEXWB_REPLACES_AMRWB "" OFF)
option(WITH_SPEEXWB_REPLACES_AMRNB "" OFF)
option(WITH_SPEEXWB_REPLACES_G726WB "" ON)

#include(CMakeLists-phamrplugin.txt)
#include(CMakeLists-phspeexplugin.txt)

if (APPLE)
	ow_create_static_library(phapi)
else (APPLE)
	ow_create_shared_library(phapi)
endif (APPLE)

ow_use_private_libraries(
	osip2
	ortp
	ilbc
	stun
	gsm
	owutil
	phapi-util
	fidlib
	speexec
	wtimer
	svoip
)

ow_use_public_libraries(
	# Should be private but phapi-old.h includes phcall.h that includes eXosip.h
	exosip
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

if (MSVC)
	ow_use_private_libraries(
		msdirent
	)
endif (MSVC)

ow_add_private_definitions(
	-DUSE_PCM=1
	-DNEED_ECMUX=1
	-DMINIMIZE_COPYING=1
	-DSCHEDULING_MODE=0
	-DBLOCKING_MODE=0
	-DDO_ECHO_CAN=1
	-DENABLE_AMR_WB=1
	-DENABLE_AMR=1
	-DUSE_WAVEIN_CBK=1
	-DPSEUDO_AEC=0
	-DNO_ECHO__SUPPRESSOR=1
	-DENABLE_ILBC=1
	-DOSIP_MT=1
	-DENABLE_TRACE=0
	-DPH_VIDEO_USELOCK=1
	-DUSE_SPXEC=1
	-DPH_FORCE_16KHZ=1
)

if (MSVC)
	ow_add_private_definitions(
		-DWENGOPHONE_UA="""wengo/v1/wengophoneng/wengo/rev${SVN_REVISION}/trunk/"""
	)
else (MSVC)
	ow_add_private_definitions(
		-DWENGOPHONE_UA='"wengo/v1/wengophoneng/wengo/rev${SVN_REVISION}/trunk/"'
	)
endif (MSVC)

if (WITH_BUILDID)
	ow_add_private_definitions(
		-DSUBVERSION_REVISION="${SVN_REVISION}"
	)
endif (WITH_BUILDID)

# PH_FORCE_16KHZ : activate FORCED 16 KHZ SAMPLING
# SUBVERSION_REVISION : grab the svn revision number
# WENGOPHONE_UA : set the wengophone user agent. It should
#                 be later changed to "Wengophone NG"

ow_add_sources(
	aec2.cpp
	owpl_plugin.c
	phapi-old.c
	phapi.c
	phaudiodriver.c
	phcodec.c
	phevents.c
	phlog.c
	phmbuf.c
	phmedia-audio-file.c
	phmedia-audio-null.c
	phmedia-audio.c
	phmedia.c
	phrecorder.c
	phresample.c
	phvline.c
	tonegen.c
	wav.c
)

if (WITH_SPEEXWB_REPLACES_G726WB)
	ow_add_private_definitions(
		-DPH_SPEEXWB_REPLACES_G726WB
	)
else (WITH_SPEEXWB_REPLACES_G726WB)
	if (WITH_SPEEXWB_REPLACES_AMRNB)
		ow_add_private_definitions(
			-DPH_SPEEXWB_REPLACES_AMRNB
		)
	else (WITH_SPEEXWB_REPLACES_AMRNB)
		if (WITH_SPEEXWB_REPLACES_AMRWB)
			ow_add_private_definitions(
				-DPH_SPEEXWB_REPLACES_AMRWB
			)
		endif (WITH_SPEEXWB_REPLACES_AMRWB)
	endif (WITH_SPEEXWB_REPLACES_AMRNB)
endif (WITH_SPEEXWB_REPLACES_G726WB)

# embedded
option(WITH_EMBEDDED_PHAPI "" OFF)
if (WITH_EMBEDDED_PHAPI)
	ow_add_private_definitions(
		-DSCHEDULING_MODE
	)
endif (WITH_EMBEDDED_PHAPI)

option(WITH_QOS_DEBUG "Build with QOS debug support" OFF)
if (WITH_QOS_DEBUG)
	ow_add_private_definitions(
		-DPHAPI_QOS_DEBUG
	)
endif (WITH_QOS_DEBUG)

option(WITH_HTTPTUNNEL "Build phapi with httptunnel support" ON)
if (WITH_HTTPTUNNEL)
	ow_use_private_libraries(
		httptunnel
	)

	ow_add_private_definitions(
		-DUSE_HTTP_TUNNEL
	)
endif (WITH_HTTPTUNNEL)

#if (WITH_VIDEO)
	ow_use_private_libraries(
		owwebcam
	)

	ow_add_private_definitions(
		-DPHAPI_VIDEO_SUPPORT
	)

	ow_add_sources(
		phcodec-h263.c
		phcodec-h264.c
		phcodec-mpeg4.c
		phmedia-video.c
		phmedia-video-control.c
		phmedia-video-bwcontrol.c
		phcodec-avcodec-wrapper.c
	)
#endif (WITH_VIDEO)

# system specific phapi options

if (WIN32)
	ow_add_private_libraries(
		winmm
		ws2_32
		user32
		shell32
		advapi32
		wininet
		iphlpapi
		strmiids
		ole32
	)

	ow_add_sources(
		phmedia-winmm.c
	)

	if (MSVC)
		ow_add_private_definitions(
			-DT_MSVC
		)

		ow_add_private_libraries(
			comsupp
		)
	else (MSVC)
		ow_add_private_libraries(
			gdi32
		)
	endif (MSVC)
endif (WIN32)

if (UNIX AND NOT WIN32)
	if (APPLE)
		ow_add_sources(
			phmedia-coreaudio.c
		)
	else (APPLE)
		ow_use_private_libraries(
			glib2
		)

		ow_add_private_libraries(
			pthread
			rt
		)

		option(WITH_SAMPLERATE "Build phapi with samplerate support" OFF)
		if (WITH_SAMPLERATE)
			ow_use_private_libraries(
				samplerate
			)

			ow_add_private_definitions(
				-DPH_USE_RESAMPLE
			)
		endif (WITH_SAMPLERATE)

		option(WITH_PHAPI_AUDIO_ALSA "Build phapi with alsa support" ON)
		if (WITH_PHAPI_AUDIO_ALSA)
			find_package(Alsa REQUIRED)

			ow_add_private_libraries(
				${ASOUND_LIBRARY}
			)

			ow_add_private_definitions(
				-DENABLE_ALSA
			)

			ow_add_sources(
				phmedia-alsa.c
			)
		endif (WITH_PHAPI_AUDIO_ALSA)

		option(WITH_PHAPI_AUDIO_ARTS "Build phapi with arts support" OFF)
		if (WITH_PHAPI_AUDIO_ARTS)
			find_package(ARTS REQUIRED)

			ow_use_private_libraries(
				ARTS
			)

			ow_add_private_definitions(
				-DENABLE_ARTS
			)

			ow_add_sources(
				phmedia-audio-arts.c
			)
		endif (WITH_PHAPI_AUDIO_ARTS)

		option(WITH_PHAPI_AUDIO_ESD "Build phapi with esd support" OFF)
		if (WITH_PHAPI_AUDIO_ESD)
			find_package(ESD REQUIRED)

			ow_use_private_libraries(
				ESD
			)

			ow_add_private_definitions(
				-DENABLE_ESD
			)

			ow_add_sources(
				phmedia-audio-esd.c
			)
		endif (WITH_PHAPI_AUDIO_ESD)
	endif (APPLE)
endif (UNIX AND NOT WIN32)

if (NOT APPLE)
	ow_use_private_libraries(
		PORTAUDIO
	)

	ow_add_private_definitions(
		-DENABLE_PORTAUDIO
	)

	ow_add_sources(
		phmedia-portaudio.c
	)
endif (NOT APPLE)

if (APPLE)
	ow_use_public_frameworks(
		AudioUnit
		CoreAudio
		AudioToolbox
		CoreServices
	)
endif (APPLE)

ow_set_library_version(0.2.0 0)

ow_create_binary()
