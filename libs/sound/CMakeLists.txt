project(owsound)

find_package(Portaudio REQUIRED)

# needed include directories to build owsound
# saves the variable in internal cache for later use
set(OWSOUND_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWTHREAD_INCLUDE_DIRS}
  CACHE INTERNAL "owsound include directories"
)

# owsound lib and dependencies
# saves the variable in internal cache for later use
set(OWSOUND_LIBRARIES
  owsound
  ${OWTHREAD_LIBRARIES}
  CACHE INTERNAL "owsound libraries"
)

include_directories(
  ${OWSOUND_INCLUDE_DIRS}
)

set(owsound_SRCS
  src/AudioDevice.cpp
  src/VolumeControl.cpp
  src/EnumDeviceType.cpp
)

if (MSVC)
  set(OWSOUND_LIBRARIES
    ${OWSOUND_LIBRARIES}
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
    set_target_properties(owsound
      PROPERTIES
        LINK_FLAGS
          -framework CoreAudio
          -framework Cocoa
    )
  else (APPLE)
    set(OWSOUND_LIBRARIES
      ${OWSOUND_LIBRARIES}
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
    set(OWSOUND_LIBRARIES
      ${OWSOUND_LIBRARIES}
      ${PORTAUDIO_LIBRARIES}
    )

    set(owsound_SRCS
      ${owsound_SRCS}
      src/win32/AudioDeviceManager.cpp
    )
  endif (WIN32)
endif (WITH_PORTAUDIO AND WIN32)

add_library(owsound STATIC ${owsound_SRCS})

target_link_libraries(${OWSOUND_LIBRARIES})

