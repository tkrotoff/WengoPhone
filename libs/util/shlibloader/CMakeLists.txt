project(owshlibloader)

#owutil dependency
link_directories(${owutil_BINARY_DIR})
include_directories(${owutil_INCLUDE_DIR})

#includes
set(owshlibloader_INCLUDE_DIR
	${owshlibloader_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owshlibloader_INCLUDE_DIR})

#sources
set(owshlibloader_SRCS
	src/SharedLibLoader.cpp
)

if (WIN32)
	set(owshlibloader_SRCS ${owshlibloader_SRCS}
		src/win32/Win32SharedLibLoader.cpp
	)
endif (WIN32)

if (UNIX)
	set(owshlibloader_SRCS ${owshlibloader_SRCS}
		src/posix/PosixSharedLibLoader.cpp
	)
endif (UNIX)

#owshlibloader
add_library(owshlibloader STATIC ${owshlibloader_SRCS})

#owutil dependency
target_link_libraries(owshlibloader owutil)
