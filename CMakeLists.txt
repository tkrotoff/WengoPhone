project(wengophone)

# Global variables
set(APPLICATION_NAME WengoPhone)
set(APPLICATION_VERSION "2.0")
string(TOLOWER ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE} BUILD_DIR)

# Where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/cmake/Modules
	${CMAKE_SOURCE_DIR}/cmake/Modules/owbuild
)

# Prefix for installing files
set(CMAKE_INSTALL_PREFIX
	${CMAKE_SOURCE_DIR}/build
)

# Add definitions
include(DefineWengoDefaults)
include(DefineCompilerFlags)
include(DefineWengoOptions)
include(DefineInstallationPaths)

# Add macros
include(MacroEnsureOutOfSourceBuild)

include(cmake/Modules/owbuild/CMakeLists.txt)

# Disallow in-source build
macro_ensure_out_of_source_build("WengoPhone requires an out of source build. Please create a separate build directory and run 'cmake path_to_wengophone [options]' there.")

# Get svn revision
ow_get_subversion_revision(SVN_REVISION)
# Bugfix with svn revision number that can integrate a : and
# this does not work under Windows for the installer name, replace it by #
string(REPLACE ":" "#" SVN_REVISION ${SVN_REVISION})

# Global requirements
set(QT_MIN_VERSION 4.1.2)
find_package(Qt4 REQUIRED)
find_package(Boost REQUIRED)

if (WIN32)
	find_package(OpenSSLInternal REQUIRED)
	find_package(GAIMInternal REQUIRED)
	find_package(FFMPEGInternal REQUIRED)
	find_package(GLIB2Internal REQUIRED)
	#find_package(PortaudioInternal REQUIRED)
else (WIN32)
	find_package(OpenSSL REQUIRED)
	find_package(GLIB2 REQUIRED)

	if (APPLE)
		find_package(FFMPEGInternal REQUIRED)
	else (APPLE)
		find_package(FFMPEG REQUIRED)
		find_package(UUID REQUIRED)
		find_package(Portaudio REQUIRED)
		find_package(Speex REQUIRED)

		if (WITH_PHAPI_AUDIO_ALSA)
			find_package(Alsa REQUIRED)
		endif (WITH_PHAPI_AUDIO_ALSA)

		if (WITH_PHAPI_AUDIO_ESD)
			find_package(ESD REQUIRED)
		endif (WITH_PHAPI_AUDIO_ESD)
	endif (APPLE)
endif (WIN32)

if (NOT MSVC)
	find_package(XML2 REQUIRED)
endif (NOT MSVC)


if (WIN32)
	subdirs(
		libs/3rdparty/portaudio
	)
endif (WIN32)

if (UNIX)
	if (NOT APPLE)
		subdirs(
			libs/3rdparty/samplerate
		)
	endif (NOT APPLE)
endif (UNIX)


subdirs(
	libs/3rdparty/glib
	libs/3rdparty/ffmpeg
	libs/3rdparty/gaim
	libs/3rdparty/boost
	libs/3rdparty/qt4
	libs/3rdparty/tinyxml
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
