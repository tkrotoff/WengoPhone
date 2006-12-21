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
	libs/3rdparty
)

if (WIN32)
	subdirs(
		crashreport
	)
endif (WIN32)
