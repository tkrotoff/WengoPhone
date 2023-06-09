ow_create_shared_library(owsystem)

ow_use_public_libraries(
	owutil
)

ow_add_sources(
	src/RegisterProtocol.cpp
	src/WebBrowser.cpp
)

if (WIN32)
	ow_add_sources(
		src/win/Startup.cpp
		src/win/Process.cpp
		src/win/WindowsVersion.cpp
		src/win/WindowsRegistry.cpp
	)
endif (WIN32)

if (UNIX)
	if (APPLE)
		ow_use_private_frameworks(
			Cocoa
		)

		ow_add_sources(
			src/null/Process.cpp
			src/mac/Startup.cpp
		)
	else (APPLE)
		ow_add_sources(
			src/unix/Process.cpp
			src/null/Startup.cpp
		)
	endif (APPLE)
endif (UNIX)

ow_create_project_binary()

if (APPLE)
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
			COMPILE_FLAGS "-x objective-c++"
	)
endif (APPLE)

subdirs(
	tests
)
