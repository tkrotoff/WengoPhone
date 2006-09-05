project(owsystem)

#owutil dependency
link_directories(${owutil_BINARY_DIR})

#includes
set(owsystem_INCLUDE_DIR
	${owsystem_SOURCE_DIR}/include
	${owutil_INCLUDE_DIR}
	CACHE INTERNAL ""
)

include_directories(${owsystem_INCLUDE_DIR})

#sources
set(owsystem_SRCS
	src/RegisterProtocol.cpp
)

if (WIN32)
	set(owsystem_SRCS ${owsystem_SRCS}
		src/win/Startup.cpp
		src/win/Processes.cpp
		src/win/WindowsVersion.c
		src/win/WindowsRegistry.cpp
	)
endif (WIN32)

if (UNIX)
	if (APPLE)
		set(owsystem_SRCS ${owsystem_SRCS}
			src/null/Processes.cpp
			src/mac/Startup.mm
		)
	else (APPLE)
		set(owsystem_SRCS ${owsystem_SRCS}
			src/unix/Processes.cpp
			src/null/Startup.cpp
		)
	endif (APPLE)
endif (UNIX)

#owsystem
add_library(owsystem STATIC ${owsystem_SRCS})

#owutil dependency
target_link_libraries(owsystem owutil)
