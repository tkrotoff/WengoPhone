project(owutil)

#Boost dependency
find_package(Boost REQUIRED)

#owcutil dependency
link_directories(${owcutil_BINARY_DIR})

#includes
set(owutil_INCLUDE_DIR
	${owutil_SOURCE_DIR}/include
	${owcutil_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	CACHE INTERNAL ""
)

include_directories(${owutil_INCLUDE_DIR})

#sources
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

#owutil
add_library(owutil STATIC ${owutil_SRCS})

#owcutil dependency
target_link_libraries(owutil owcutil)
