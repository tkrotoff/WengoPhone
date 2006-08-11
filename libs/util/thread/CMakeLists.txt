project(owthread)

#owutil dependency
link_directories(${owutil_BINARY_DIR})

#includes
set(owthread_INCLUDE_DIR
	${owthread_SOURCE_DIR}/include
	${owutil_INCLUDE_DIR}
	CACHE INTERNAL ""
)

include_directories(${owthread_INCLUDE_DIR})

#sources
set(owthread_SRCS
	src/Timer.cpp
	src/Thread.cpp
)

#owthread
add_library(owthread STATIC ${owthread_SRCS})

#owutil dependency
target_link_libraries(owthread owutil)
