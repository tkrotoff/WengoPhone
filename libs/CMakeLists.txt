if (WIN32)
  subdirs(
    portaudio
  )
else (WIN32)
  subdirs(
    gaim
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

