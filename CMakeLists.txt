project(wengophone)

# global needed variables
set(APPLICATION_NAME ${PROJECT_NAME})

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

include(DefineWengoDefaults)
include(DefineCompilerFlags)

#set(CMAKE_START_TEMP_FILE "")
#set(CMAKE_END_TEMP_FILE "")
#set(CMAKE_VERBOSE_MAKEFILE 1)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

#Compatibility between Qt4 and Boost
add_definitions(-DQT_NO_KEYWORDS)

set(ENABLE_SSL $ENV{ENABLE_SSL} CACHE INTERNAL "")
set(ENABLE_SSL true CACHE INTERNAL "")

subdirs(
  libs
)
