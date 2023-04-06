if (LINUX)
	option(FFMPEG_INTERNAL "Use internal FFmpeg" OFF)
	option(SPEEX_INTERNAL "Use internal Speex" OFF)
	option(PORTAUDIO_INTERNAL "Use internal PortAudio" ON)
	option(IAXCLIENT_INTERNAL "Use internal iaxclient" ON)
	option(SAMPLERATE_INTERNAL "Use internal libsamplerate" OFF)
	option(OSIP2_INTERNAL "Use internal oSIP" ON)
	option(CURL_INTERNAL "Use internal cURL" OFF)
else (LINUX)
	option(PORTAUDIO_INTERNAL "Use internal PortAudio" ON)
	option(FFMPEG_INTERNAL "Use internal FFmpeg" ON)
	option(SPEEX_INTERNAL "Use internal Speex" ON)
	option(IAXCLIENT_INTERNAL "Use internal iaxclient" ON)
	option(SAMPLERATE_INTERNAL "Use internal libsamplerate" ON)
	option(OSIP2_INTERNAL "Use internal oSIP" ON)
	option(CURL_INTERNAL "Use internal cURL" ON)
endif (LINUX)

if (WIN32)
	if (FFMPEG_OLD_VERSION)
		subdirs(
			ffmpeg-bin-win32
		)
	else (FFMPEG_OLD_VERSION)
		subdirs(
			ffmpeg
		)
	endif (FFMPEG_OLD_VERSION)
else (WIN32)
	subdirs(
		ffmpeg
	)
endif (WIN32)

subdirs(
	pthread
	libstdcpp
	python
	boost
	boost-test
	boost-python

	#qt4

	qtcore
	qtgui
	qt3support
	qtactivex
	qtdbus
	qtnetwork
	qtopengl
	qtscript
	qtsql
	qtsvg
	qttest
	qtxml

	openssl
	libxml
)

if (COIPMANAGER_INTERNAL)
	subdirs(
		glib
		curl
		gnutls
		libpurple
	)
else (COIPMANAGER_INTERNAL)
	subdirs(
		coipmanager
	)
endif (COIPMANAGER_INTERNAL)

subdirs(
	alsa
	arts
	esd
	#ffmpeg
	tinyxml
	uuid
	x11
	samplerate
	sndfile
	#iaxclient
	#sdl
	sip
)

if (LINUX)
	subdirs(
		coredumper
	)
endif (LINUX)

if (NOT APPLE)
	subdirs(
		portaudio
	)
endif (NOT APPLE)
