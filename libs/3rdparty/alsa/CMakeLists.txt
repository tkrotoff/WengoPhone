if (UNIX AND NOT APPLE)

	if (NOT ASOUND_LIBRARY)
		find_package(Alsa REQUIRED)
	endif (NOT ASOUND_LIBRARY)

	ow_create_project(alsa)

	ow_add_public_libraries(
		${ASOUND_LIBRARY}
	)

	ow_add_public_include_dirs(
		${CMAKE_CURRENT_SOURCE_DIR}
	)

endif (UNIX AND NOT APPLE)
