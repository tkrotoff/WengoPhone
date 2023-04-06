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
	#pthread
	libstdcpp
	boost
	boost-test
	qt4
	glib
	openssl
	curl
	alsa
	arts
	esd
#	ffmpeg
	tinyxml
	libxml
	gnutls
	gaim
	uuid
	x11
	python
	samplerate
	sndfile
	#iaxclient
)

if (LINUX)
	subdirs(
		coredumper
	)
endif (LINUX)
