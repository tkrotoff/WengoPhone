# - OW_FINISH_BINARY()
# Finishes the creation of a binary file, used by OWCreateSharedLibrary(), OWCreateExecutable()...
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_FINISH_BINARY)

	ow_check_project()

	ow_unique(${PROJECT_NAME}_LIBRARIES ${${PROJECT_NAME}_LIBRARIES} ${${PROJECT_NAME}_PRIVATE_LIBRARIES})
	target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARIES})

	if (${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS)
		ow_unique(${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS ${${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS})

		string(REPLACE ";" " " ${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS "${${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS}")

		set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS})
	endif (${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS)

	if (${PROJECT_NAME}_PUBLIC_LINK_FLAGS OR ${PROJECT_NAME}_PRIVATE_LINK_FLAGS)
		ow_unique(unique ${${PROJECT_NAME}_PUBLIC_LINK_FLAGS} ${${PROJECT_NAME}_PRIVATE_LINK_FLAGS})

		string(REPLACE ";" " " unique "${unique}")

		set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${unique})
	endif (${PROJECT_NAME}_PUBLIC_LINK_FLAGS OR ${PROJECT_NAME}_PRIVATE_LINK_FLAGS)

	if (${PROJECT_NAME}_BUILD_VERSION AND ${PROJECT_NAME}_API_VERSION)
		set_target_properties(${PROJECT_NAME}
			PROPERTIES
				VERSION ${${PROJECT_NAME}_BUILD_VERSION}
				SOVERSION ${${PROJECT_NAME}_API_VERSION}
		)
	endif (${PROJECT_NAME}_BUILD_VERSION AND ${PROJECT_NAME}_API_VERSION)

	ow_project_log()

	if (CMAKE_BUILD_TYPE MATCHES Debug)
		install(TARGETS ${PROJECT_NAME} DESTINATION debug)
	else (CMAKE_BUILD_TYPE MATCHES Debug)
		install(TARGETS ${PROJECT_NAME} DESTINATION release)
	endif (CMAKE_BUILD_TYPE MATCHES Debug)

endmacro (OW_FINISH_BINARY)
