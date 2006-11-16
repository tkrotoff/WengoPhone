ow_create_static_library(owpixertool)

ow_use_public_libraries(
	owcutil
	ffmpeg
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
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

ow_create_binary()
