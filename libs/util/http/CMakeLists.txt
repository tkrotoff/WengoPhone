project(owhttp)

#owutil dependency
link_directories(${owutil_BINARY_DIR})
include_directories(${owutil_INCLUDE_DIR})

#owcurl dependency
link_directories(${owcurl_BINARY_DIR})
include_directories(${owcurl_INCLUDE_DIR})

#owthread dependency
include_directories(${owthread_INCLUDE_DIR})
link_directories(${owthread_BINARY_DIR})

#includes
set(owhttp_INCLUDE_DIR
	${owhttp_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owhttp_INCLUDE_DIR})

#sources
set(owhttp_SRCS
	src/HttpRequest.cpp
	src/null/NullHttpRequest.cpp
	src/curl/CurlHttpRequest.cpp
	#src/qt/QtHttpRequest.cpp
)

#owhttp
add_library(owhttp STATIC ${owhttp_SRCS})

#owutil dependency
target_link_libraries(owhttp owutil)
