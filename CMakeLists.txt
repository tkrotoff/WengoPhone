project(wengophone)

# Global needed variables
if (NOT APPLE)
	set(APPLICATION_NAME ${PROJECT_NAME})
else (NOT APPLE)
	set(APPLICATION_NAME WengoPhone)
endif (NOT APPLE)
set(APPLICATION_VERSION "2.0")

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
include(MacroGenerateQtQmFiles)

include(cmake/Modules/owbuild/CMakeLists.txt)

# Disallow in-source build
macro_ensure_out_of_source_build("WengoPhone requires an out of source build. Please create a separate build directory and run 'cmake path_to_wengophone [options]' there.")

# Get svn revision
ow_get_subversion_revision(SVN_REVISION)

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
	if (APPLE)
		find_package(FFMPEGInternal REQUIRED)
	else (APPLE)
		find_package(FFMPEG REQUIRED)
	endif (APPLE)
	find_package(OpenSSL REQUIRED)
	find_package(GLIB2 REQUIRED)
endif (WIN32)

if (NOT MSVC)
	find_package(XML2 REQUIRED)
endif (NOT MSVC)

if (NOT APPLE AND NOT WIN32)
	find_package(Portaudio REQUIRED)
endif (NOT APPLE AND NOT WIN32)

if (UNIX AND NOT WIN32 AND NOT APPLE)
	find_package(UUID REQUIRED)
endif (UNIX AND NOT WIN32 AND NOT APPLE)

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
