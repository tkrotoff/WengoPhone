if (WIN32)
  subdirs(
    portaudio
  )
endif (WIN32)

if (UNIX)
  if (NOT APPLE)
    subdirs(
      samplerate
    )
  endif (NOT APPLE)
endif (UNIX)

subdirs(
  gaim
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
  skinstyle
  sound
  timer
)
