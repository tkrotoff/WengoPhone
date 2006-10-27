ow_create_project(owpixertool)

if (WIN32)
	find_package(FFMPEGInternal REQUIRED)
else (WIN32)
	find_package(FFMPEG REQUIRED)
endif (WIN32)

ow_use_public_libraries(
	owcutil
	FFMPEG
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

#ow_add_public_definitions(
#	-DOWPIXERTOOL_DLL
#)

ow_add_private_definitions(
	-DOWPIXERTOOL_DLL
	-DBUILD_OWPIXERTOOL_DLL
)

ow_add_sources(
	src/pixertool.cpp
	src/ffmpeg/ffmpeg-pixertool.c
)

if (MSVC)
	ow_add_private_libraries(
		winmm
		strmiids
		comsupp
	)

	ow_add_sources(
		src/directx/directx-pixertool.cpp
	)
endif (MSVC)

if (UNIX AND NOT WIN32)
	if (APPLE)
		ow_add_sources(
			src/quicktime/quicktime-pixertool.c
		)
	else (APPLE)
		ow_add_sources(
			src/v4l/v4l-pixertool.c
		)
	endif(APPLE)
endif (UNIX AND NOT WIN32)

ow_create_static_library()
