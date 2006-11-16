ow_create_project(glib2)

ow_use_public_libraries(
	GLIB2
)

if (MSVC)
	ow_copy_file(${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/libglib-2.0-0.dll .)
    ow_copy_file(${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/libgmodule-2.0-0.dll .)
    ow_copy_file(${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/libgthread-2.0-0.dll .)
endif (MSVC)
