if (UNIX)

	if (NOT UUID_FOUND)
		find_package(UUID REQUIRED)
	endif (NOT UUID_FOUND)

	ow_create_project(uuid)

	ow_use_public_libraries(
		UUID
	)

endif (UNIX)
