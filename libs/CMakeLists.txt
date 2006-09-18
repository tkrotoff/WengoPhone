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
  samplerate
  skinstyle
  sound
  timer
  imwrapper
  sipwrapper
)

if (WIN32)
  subdirs(
    portaudio
  )
else (WIN32)
  subdirs(
    gaim
  )
endif (WIN32)
