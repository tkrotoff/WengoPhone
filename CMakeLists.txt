project(wengophone)

# global needed variables
set(APPLICATION_NAME ${PROJECT_NAME})

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(DefineWengoDefaults)
include(DefineCompilerFlags)
include(DefineWengoOptions)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Compatibility between Qt4 and Boost
add_definitions(-DQT_NO_KEYWORDS)

# Global requirements
set(QT_MIN_VERSION 4.1.2)
find_package(Qt4 REQUIRED)

if (UNIX AND NOT WIN32)
  find_package(GTK2 REQUIRED)
endif (UNIX AND NOT WIN32)

subdirs(
  libs
)
