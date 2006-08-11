project(owsocket)

#owutil dependency
link_directories(${owutil_BINARY_DIR})
include_directories(${owutil_INCLUDE_DIR})

#owthread dependency
link_directories(${owthread_BINARY_DIR})
include_directories(${owthread_INCLUDE_DIR})

#includes
set(owsocket_INCLUDE_DIR
	${owsocket_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owsocket_INCLUDE_DIR})

#sources
set(owsocket_SRCS
	src/ServerSocket.cpp
	src/ClientSocket.cpp
)

#owsocket
add_library(owsocket STATIC ${owsocket_SRCS})

#owutil dependency
target_link_libraries(owsocket owutil)
