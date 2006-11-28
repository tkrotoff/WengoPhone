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

# Add definitions/options/default values...
include(DefineWengoDefaults.cmake)
include(DefineCompilerFlags.cmake)
include(DefineWengoOptions.cmake)
include(DefineInstallationPaths.cmake)

# Add macros
include(MacroEnsureOutOfSourceBuild)

include(cmake/Modules/owbuild/CMakeLists.txt)

# Disallow in-source build
macro_ensure_out_of_source_build("WengoPhone requires an out of source build. Please create a separate build directory and run 'cmake path_to_wengophone [options]' there.")

# Gets svn revision
set(SVN_REVISION "0")
ow_get_subversion_revision(SVN_REVISION)
# Bugfix with svn revision number that can integrate a : and
# this does not work under Windows for the installer name, replace it by #
string(REPLACE ":" "#" SVN_REVISION ${SVN_REVISION})


if (NOT WIN32)
	find_package(UUID REQUIRED)
	find_package(XML2 REQUIRED)
endif (NOT WIN32)

if (UNIX)
	if (NOT APPLE)
		subdirs(
			libs/3rdparty/samplerate
		)
	endif (NOT APPLE)
endif (UNIX)

subdirs(
	libs/3rdparty/boost
	libs/3rdparty/qt4
	libs/3rdparty/glib
	libs/3rdparty/openssl
	libs/3rdparty/portaudio
	libs/3rdparty/ffmpeg
	libs/3rdparty/tinyxml
	libs/3rdparty/gaim
	#libs/3rdparty/samplerate

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
