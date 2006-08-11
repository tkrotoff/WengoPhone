project(owwenbox)

#owutil dependency
include_directories(${owutil_INCLUDE_DIR})
link_directories(${owutil_BINARY_DIR})

#owshlibloader dependency
include_directories(${owshlibloader_INCLUDE_DIR})
link_directories(${owshlibloader_BINARY_DIR})

#includes
set(owwenbox_INCLUDE_DIR
	${owwenbox_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owwenbox_INCLUDE_DIR})

#sources
set(owwenbox_SRCS
	src/Wenbox.cpp
)

#owwenbox
add_library(owwenbox STATIC ${owwenbox_SRCS})

#owutil dependency
target_link_libraries(owwenbox owutil)

subdirs(src/null)
if (WIN32)
	subdirs(src/yealink)
endif (WIN32)
