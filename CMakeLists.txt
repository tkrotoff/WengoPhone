project(wengophone)

# Global variables
set(APPLICATION_NAME WengoPhone)
set(APPLICATION_VERSION "2.0")

# Where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/owbuild
	${CMAKE_SOURCE_DIR}/owbuild/owbuild
)

# Add definitions/options/default values...
include(DefineWengoDefaults.cmake)
include(DefineCompilerFlags.cmake)
include(DefineWengoOptions.cmake)
include(DefineInstallationPaths.cmake)

# Add macros
include(owbuild/owbuild/CMakeLists.txt)

# Disallow in-source build
ow_ensure_out_of_source_build()

include(owbuild/owbuild/OWInitializationInfo.cmake)

if (NOT APPLE)
	subdirs(
		libs/3rdparty/portaudio
	)
endif (NOT APPLE)

subdirs(
	libs/3rdparty/boost
	libs/3rdparty/boost-test
	libs/3rdparty/qt4
	libs/3rdparty/glib
	libs/3rdparty/openssl
	libs/3rdparty/alsa
	libs/3rdparty/arts
	libs/3rdparty/esd
	libs/3rdparty/ffmpeg
	libs/3rdparty/tinyxml
	libs/3rdparty/libxml
	libs/3rdparty/gaim
	libs/3rdparty/uuid
	libs/3rdparty/x11
	libs/3rdparty/python
	libs/3rdparty/samplerate

	libs/owcurl
	libs/util
	libs/wenbox
	libs/qtutil
	libs/idle
	libs/owbrowser
	libs/trayicon
	libs/softupdater
	libs/pixertool
	libs/webcam
	libs/sound
	libs/timer
	wifo
	libs/imwrapper
	libs/imwrapper/src/gaim
	libs/sipwrapper
	libs/sipwrapper/src/phapi
	libs/imwrapper/src/multiim
	wengophone
)

if (WIN32)
	subdirs(
		crashreport
	)
endif (WIN32)
