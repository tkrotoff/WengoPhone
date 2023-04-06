include(CMakeLists-owbuild.txt)

project(wengophone)

# Global variables
set(APPLICATION_NAME WengoPhone)
set(APPLICATION_VERSION "3.0")

# Add options values
include(DefineWengoOptions.cmake)
include(DefineInstallationPaths.cmake)

if (MSVC80)
	# Disable secure CRT warnings under Visual C++ 2005
	# See http://msdn.microsoft.com/vstudio/tour/vs2005_guided_tour/VS2005pro/Framework/CPlusSafeLibraries.htm
	# Disable deprecated POSIX-compliant functions warnings under Visual C++ 2005
	add_definitions(
		-D_CRT_SECURE_NO_DEPRECATE
		-D_CRT_NONSTDC_NO_DEPRECATE
	)
endif (MSVC80)

subdirs(
	libs/3rdparty
)

if (COIPMANAGER_INTERNAL)
	subdirs(
		libs/owutil
		libs/qtcoreutil
		libs/qtutil
		libs/pixertool
		libs/webcam
		libs/timer
		libs/sound
		wifo
		libs/networkdiscovery
		libs/coipmanager_base
		libs/sipwrapper
		libs/sipwrapper/src/phapi
		#libs/sipwrapper/src/iaxclient
		libs/purplewrapper
		libs/webservice
		libs/ssorequest
		libs/coipmanager
		libs/coipmanager/plugins
		libs/coipmanager_threaded
		libs/coipmanager_bindings
		libs/coipmanager_sdk
		libs/qtevent
	)
endif (COIPMANAGER_INTERNAL)

subdirs(
	libs/wenbox
	libs/webcam/tests
	libs/idle
	libs/ftpupload
	libs/owbrowser
	libs/trayicon
	libs/softupdater
	pywengophone
	wengochat
	wengocall
	wengoconnection
)

if (WIN32)
	subdirs(
		crashreport
	)
endif (WIN32)

# This target MUST be the last one. It gathers all unit-tests available
# in above targets.
subdirs(
	unit-tests
)

# For Qt-4 translations

add_custom_target(lupdate
	COMMAND
		"${CMAKE_COMMAND}"
		-DPRO_FILE="${CMAKE_CURRENT_SOURCE_DIR}/lang.pro"
		-P "${CMAKE_CURRENT_SOURCE_DIR}/owbuild/owbuild/OWLocaleUpdate.cmake"
	WORKING_DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT
		"Runs lupdate command"
)

add_custom_target(lrelease
	COMMAND
		"${CMAKE_COMMAND}"
		-DLOCALE_COPY_DIR="${LOCALE_COPY_DIR}"
		-P "${CMAKE_CURRENT_SOURCE_DIR}/owbuild/owbuild/OWLocaleRelease.cmake"
	WORKING_DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT
		"Runs lrelease command"
)
