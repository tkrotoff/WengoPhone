project(owsound)

# needed include directories to build owsound
# saves the variable in internal cache for later use
set(OWSOUND_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWUTIL_INCLUDE_DIRS}
  ${OWTHREAD_INCLUDE_DIRS}
  CACHE INTERNAL "owsound include directories"
)

set(OWSOUND_LIBRARY
  owsound
  CACHE INTERNAL "owsound library"
)

# owsound lib and dependencies
set(OWSOUND_LINK_LIBRARIES
  ${OWSOUND_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWTHREAD_LIBRARY}
)

set(owsound_SRCS
  src/AudioDevice.cpp
  src/VolumeControl.cpp
  src/EnumDeviceType.cpp
)

if (MSVC)
  set(OWSOUND_LINK_LIBRARIES
    ${OWSOUND_LINK_LIBRARIES}
    winmm
    advapi32
  )

  set(owsound_SRCS
    ${owsound_SRCS}
    src/win32/Sound.cpp
    src/win32/Win32VolumeControl.cpp
    src/win32/Win32AudioDevice.cpp
    src/win32/SoundThread.cpp
    src/win32/Win32AudioDeviceId.cpp
    src/win32/playsound/SoundFile.cpp
    src/win32/playsound/PlaySoundFile.cpp
  )
endif (MSVC)

if (UNIX AND NOT WIN32)
  if (APPLE)
    set(owsound_SRCS
      ${owsound_SRCS}
      src/mac/AudioDeviceManager.cpp
      src/mac/CoreAudioUtilities.cpp
      src/mac/MacAudioDevice.cpp
      src/mac/MacSound.cpp
      src/mac/MacVolumeControl.cpp
      src/mac/Sound.cpp
    )
  else (APPLE)
    set(OWSOUND_LINK_LIBRARIES
      ${OWSOUND_LINK_LIBRARIES}
      ${PORTAUDIO_LIBRARIES}
    )

    set(owsound_SRCS
      ${owsound_SRCS}
      src/unix/Sound.cpp
      src/unix/UnixVolumeControl.cpp
      src/portaudio/AudioDeviceManager.cpp
      src/portaudio/PAAudioDevice.cpp
    )
  endif (APPLE)
endif (UNIX AND NOT WIN32)

if (WITH_PORTAUDIO AND WIN32)
  set(owsound_SRCS
    ${owsound_SRCS}
    src/portaudio/AudioDeviceManager.cpp
    src/portaudio/PAAudioDevice.cpp
  )
else (WITH_PORTAUDIO AND WIN32)
  if (WIN32)
    set(OWSOUND_LINK_LIBRARIES
      ${OWSOUND_LINK_LIBRARIES}
      ${PORTAUDIO_LIBRARIES}
    )

    set(owsound_SRCS
      ${owsound_SRCS}
      src/win32/AudioDeviceManager.cpp
    )
  endif (WIN32)
endif (WITH_PORTAUDIO AND WIN32)

include_directories(
  ${OWSOUND_INCLUDE_DIRS}
)

add_library(owsound STATIC ${owsound_SRCS})

if (APPLE)
  macro_add_link_flags(${OWSOUND_LIBRARY}
    -framework CoreAudio
    -framework Cocoa
  )

  macro_add_compile_flags(${OWSOUND_LIBRARY}
    "-x objective-c++"
  )
endif (APPLE)

target_link_libraries(${OWSOUND_LINK_LIBRARIES})

