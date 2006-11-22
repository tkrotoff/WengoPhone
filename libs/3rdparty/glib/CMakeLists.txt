ow_create_project(glib2)

ow_use_public_libraries(
	GLIB2
)

if (MSVC)
	ow_copy_dir(${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/*.dll .)
endif (MSVC)
