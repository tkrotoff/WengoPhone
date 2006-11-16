ow_create_project(glib2)

ow_use_public_libraries(
	GLIB2
)

if (MSVC)
	file(GLOB dllList ${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/*.dll)
	if (dllList)
		foreach (dll ${dllList})
			ow_copy_file(${dll} .)
		endforeach (dll ${dllList})
	endif (dllList)
endif (MSVC)
