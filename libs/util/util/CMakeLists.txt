project(owutil)

#Boost dependency
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#owcutil dependency
include_directories(${owcutil_SOURCE_DIR}/include)
link_directories(${owcutil_BINARY_DIR})
if (MSVC)
	include_directories(${owcutil_SOURCE_DIR}/include/cutil/msvc)
endif (MSVC)

include_directories(include)

set(owutil_SRCS
	src/exception/Exception.cpp
	src/Base64.cpp
	src/Date.cpp
	src/File.cpp
	src/Logger.cpp
	src/Path.cpp
	src/Picture.cpp
	src/String.cpp
	src/StringList.cpp
	src/Time.cpp
	src/Uuid.cpp
	src/WebBrowser.cpp
)

if (WIN32)
	set(owutil_SRCS ${owutil_SRCS}
		src/win/UuidWin.cpp
	)
endif (WIN32)

if (APPLE)
	set(owutil_SRCS ${owutil_SRCS}
		src/mac/UuidMac.cpp
	)
endif (APPLE)

if (UNIX)
	set(owutil_SRCS ${owutil_SRCS}
		src/unix/UuidUnix.cpp
	)
endif (UNIX)

add_library(owutil STATIC ${owutil_SRCS})

#owcutil dependency
target_link_libraries(owutil owcutil)
