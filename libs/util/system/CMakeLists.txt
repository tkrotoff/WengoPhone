ow_create_project(owsystem)

ow_use_public_libraries(
	owutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

#ow_add_public_definitions(
#	-DOWSYSTEM_DLL
#)

#ow_add_private_definitions(
#	-DOWSYSTEM_DLL
#	-DBUILD_OWSYSTEM_DLL
#)

ow_add_sources(
	src/RegisterProtocol.cpp
)

if (WIN32)
	ow_add_sources(
		src/win/Startup.cpp
		src/win/Processes.cpp
		src/win/WindowsVersion.cpp
		src/win/WindowsRegistry.cpp
	)
endif (WIN32)

if (UNIX)
	if (APPLE)
		ow_add_sources(
			src/null/Processes.cpp
			src/mac/Startup.cpp
		)
	else (APPLE)
		ow_add_sources(
			src/unix/Processes.cpp
			src/null/Startup.cpp
		)
	endif (APPLE)
endif (UNIX)

ow_create_static_library()

if (APPLE)
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
			COMPILE_FLAGS "-x objective-c++"
	)
endif (APPLE)
