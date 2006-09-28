project(wengophone)

# global needed variables
set(APPLICATION_NAME ${PROJECT_NAME})
set(APPLICATION_VERSION "2.0rc3")

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# add definitions
include(DefineWengoDefaults)
include(DefineCompilerFlags)
include(DefineWengoOptions)
include(DefineInstallationPaths)

# add macros
include(MacroGetSubversionRevision)
include(MacroGenerateQtQmFiles)
include(MacroAddCompileFlags)
include(MacroAddLinkFlags)
include(MacroAddPlugin)

# get svn revision
include(MacroGetSubversionRevision)
macro_get_subversion_revision(SVN_REVISION)

# Compatibility between Qt4 and Boost
add_definitions(-DQT_NO_KEYWORDS)

# Global requirements
set(QT_MIN_VERSION 4.1.2)
find_package(Qt4 REQUIRED)
find_package(Boost REQUIRED)

if (UNIX AND NOT WIN32)
  find_package(GLIB2 REQUIRED)
endif (UNIX AND NOT WIN32)

if (WIN32)
  find_package(OpenSSLInternal REQUIRED)
  find_package(FFMPEGInternal REQUIRED)
else (WIN32)
  find_package(OpenSSL REQUIRED)
  find_package(FFMPEG REQUIRED)
endif (WIN32)

if (NOT APPLE AND NOT WIN32)
  find_package(Portaudio REQUIRED)
endif (NOT APPLE AND NOT WIN32)

subdirs(
  libs
  wifo
  libs/imwrapper
  libs/imwrapper/src/gaim
  libs/sipwrapper
  libs/sipwrapper/src/phapi
  libs/imwrapper/src/multiim
  wengophone
  crashreport
)

