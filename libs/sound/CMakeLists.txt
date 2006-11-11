ow_create_static_library(owsound)

ow_use_public_libraries(
	owthread
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/AudioDevice.cpp
	src/VolumeControl.cpp
	src/EnumDeviceType.cpp
)

if (MSVC)
	ow_add_private_libraries(
		winmm
		advapi32
	)

	ow_add_sources(
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
		ow_add_sources(
			src/mac/AudioDeviceManager.cpp
			src/mac/CoreAudioUtilities.cpp
			src/mac/MacAudioDevice.cpp
			src/mac/MacSound.cpp
			src/mac/MacVolumeControl.cpp
			src/mac/Sound.cpp
		)
	else (APPLE)
		ow_add_private_libraries(
			${PORTAUDIO_LIBRARIES}
		)

		ow_add_sources(
			src/unix/Sound.cpp
			src/unix/UnixVolumeControl.cpp
			src/portaudio/AudioDeviceManager.cpp
			src/portaudio/PAAudioDevice.cpp
		)
	endif (APPLE)
endif (UNIX AND NOT WIN32)

if (WITH_PORTAUDIO AND WIN32)
	ow_add_sources(
		src/portaudio/AudioDeviceManager.cpp
		src/portaudio/PAAudioDevice.cpp
	)
else (WITH_PORTAUDIO AND WIN32)
	if (WIN32)
		ow_add_private_libraries(
			${PORTAUDIO_LIBRARIES}
		)

		ow_add_sources(
			src/win32/AudioDeviceManager.cpp
		)
	endif (WIN32)
endif (WITH_PORTAUDIO AND WIN32)

if (APPLE)
	ow_use_public_frameworks(
		AudioToolBox
		AudioUnit
		CoreAudio
		Cocoa
	)
endif (APPLE)

ow_create_binary()

if (APPLE)
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
			COMPILE_FLAGS "-x objective-c++"
	)
endif (APPLE)
