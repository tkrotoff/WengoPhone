ow_create_project(ffmpeg)

ow_use_public_libraries(
	FFMPEG
)

if (MSVC)
	ow_copy_dir(${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/*.dll .)
endif (MSVC)
