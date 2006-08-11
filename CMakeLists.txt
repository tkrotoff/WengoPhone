project(wengophone)

#set(CMAKE_VERBOSE_MAKEFILE ON)
#set(CMAKE_START_TEMP_FILE "")
#set(CMAKE_END_TEMP_FILE "")

set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(ENABLE_SSL $ENV{ENABLE_SSL} CACHE INTERNAL "")
set(ENABLE_SSL true CACHE INTERNAL "")

subdirs(
	libs/openssl
	libs/curl
	libs/tinyxml
	libs/util
	libs/wenbox
)
