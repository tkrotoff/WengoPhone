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
)

if (WIN32)
  subdirs(
    portaudio
endif (WIN32)
