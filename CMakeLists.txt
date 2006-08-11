project(wengophone)

#set(CMAKE_START_TEMP_FILE "")
#set(CMAKE_END_TEMP_FILE "")
#set(CMAKE_VERBOSE_MAKEFILE 1)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

#Compatibility between Qt4 and Boost
add_definitions(-DQT_NO_KEYWORDS)

set(ENABLE_SSL $ENV{ENABLE_SSL} CACHE INTERNAL "")
set(ENABLE_SSL true CACHE INTERNAL "")

subdirs(
	libs/openssl
	libs/curl
	libs/tinyxml
	libs/util
	libs/wenbox
	libs/qtutil
	libs/idle
	libs/owbrowser
	libs/trayicon
	libs/softupdater
)
