if (UNIX AND NOT APPLE)

	if (NOT ALSA_FOUND)
		find_package(Alsa REQUIRED)
	endif (NOT ALSA_FOUND)

	ow_create_project(alsa)

	ow_use_public_libraries(
		ALSA
	)

endif (UNIX AND NOT APPLE)
