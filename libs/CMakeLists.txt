if (WIN32)
	subdirs(
		3rdparty/portaudio
		3rdparty/tinyxml
	)
endif (WIN32)

if (NOT MSVC)
	subdirs(
		3rdparty/gaim
	)
endif (NOT MSVC)

if (UNIX)
	if (NOT APPLE)
		subdirs(
			3rdparty/samplerate
		)
	endif (NOT APPLE)
endif (UNIX)

subdirs(
	3rdparty/qt4
	3rdparty/boost
	owcurl
	util
	wenbox
	qtutil
	idle
	owbrowser
	trayicon
	softupdater
	pixertool
	webcam
	sound
	timer
)
