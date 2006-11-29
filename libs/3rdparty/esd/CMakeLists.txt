if (UNIX AND NOT APPLE)
	if (PHAPI_AUDIO_ESD_SUPPORT)

		if (NOT ESD_FOUND)
			find_package(ESD REQUIRED)
		endif (NOT ESD_FOUND)
	
		ow_create_project(esd)
	
		ow_use_public_libraries(
			ESD
		)

	endif (PHAPI_AUDIO_ESD_SUPPORT)
endif (UNIX AND NOT APPLE)
