if (WIN32)
  option(WITH_PORTAUDIO_DSOUND "Should Portaudio compiled with DirectSound support" OFF)
endif (WIN32)

option(WITH_VIDEO "Build phapi with video support" ON)
option(WITH_BUILDID "Build wengophone with buildid (builds faster without)" ON)
