include(CMakeLists-owbuild.txt)

project(wengophone)

if (GCC41)
	message(FATAL "WengoPhone does not work with GCC 4.1.x, please use a previous version"
endif (GCC41)

# Global variables
set(APPLICATION_NAME WengoPhone)
set(APPLICATION_VERSION "2.0")

# Add options values
include(DefineWengoOptions.cmake)
include(DefineInstallationPaths.cmake)

subdirs(
	libs/3rdparty

	libs/owcurl
	libs/owutil
	libs/wenbox
	libs/qtutil
	libs/idle
	libs/owbrowser
	libs/trayicon
	libs/softupdater
	libs/owpixertool
	libs/owwebcam
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
