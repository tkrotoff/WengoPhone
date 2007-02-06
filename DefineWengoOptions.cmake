option(BUILDID_SUPPORT "Build WengoPhone/phApi with buildId (builds faster without)" OFF)

option(WENGOPHONE_RESOURCEDIR_PATH "WengoPhone: use ${DATA_INSTALL_DIR} as hardcoded resource path" ON)
option(WENGOPHONE_XV_SUPPORT "Build WengoPhone with XV hardware acceleration support" OFF)

option(SIPWRAPPER_BACKEND_SIPX "SipX backend for SipWrapper" OFF)
option(SIPWRAPPER_BACKEND_PHAPI "PhApi backend for SipWrapper" OFF)
option(IMWRAPPER_BACKEND_MULTI "PhApi and Gaim backend for IMWrapper" ON)

option(OWSOUND_PORTAUDIO_SUPPORT "Build owsound with PortAudio support" ON)

option(PHAPI_VIDEO_SUPPORT "Build phApi with video support" ON)

if (UNIX AND NOT APPLE)
	option(PHAPI_SAMPLERATE_SUPPORT "Build phApi with libsamplerate support" ON)
else (UNIX AND NOT APPLE)
	option(PHAPI_SAMPLERATE_SUPPORT "Build phApi with libsamplerate support" OFF)
endif (UNIX AND NOT APPLE)

option(PHAPI_AUDIO_ALSA_SUPPORT "Build phApi with ALSA support" ON)
option(PHAPI_AUDIO_ARTS_SUPPORT "Build phApi with aRts support" OFF)
option(PHAPI_AUDIO_ESD_SUPPORT "Build phApi with ESD support" OFF)
option(PHAPI_HTTPTUNNEL_SUPPORT "Build phApi with httptunnel support" ON)
option(PHAPI_QOS_DEBUG_SUPPORT "Build phApi with QOS debug support" OFF)
option(PHAPI_EMBEDDED_SUPPORT "Build phApi with embedded target support" OFF)
option(PHAPI_SPEEXWB_REPLACES_AMRWB_HACK "Build phApi with Speex-WB/AMR-WB hack" OFF)
option(PHAPI_SPEEXWB_REPLACES_AMRNB_HACK "Build phApi with Speex-WB/AMR-NB hack" OFF)
option(PHAPI_SPEEXWB_REPLACES_G726WB_HACK "Build phApi with Speex-WB/G726-WB hack" ON)
option(PHAPI_PYTHON_WRAPPER "Python wrapper for PhApi, used for unit testing" OFF)

option(INSTALLER_WITH_PDB "Create NSIS installer under Windows including .pdb files" ON)
