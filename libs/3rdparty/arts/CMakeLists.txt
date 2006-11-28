if (UNIX AND NOT APPLE)

	if (NOT ARTS_FOUND)
		find_package(ARTS REQUIRED)
	endif (NOT ARTS_FOUND)

	ow_create_project(arts)

	ow_use_public_libraries(
		ARTS
	)

endif (UNIX AND NOT APPLE)
