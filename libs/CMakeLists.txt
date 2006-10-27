if (WIN32)
  subdirs(
    portaudio
  )
endif (WIN32)

if (NOT MSVC)
  subdirs(
    gaim
  )
endif (NOT MSVC)

if (UNIX)
  if (NOT APPLE)
    subdirs(
      samplerate
    )
  endif (NOT APPLE)
endif (UNIX)

subdirs(
	curl
	tinyxml
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
