# - OW_PREPARE_BINARY()
# Prepares the creation of a binary file, used by OWCreateBinary().
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_PREPARE_BINARY)

	ow_check_project()

	if (NOT ${PROJECT_NAME}_PROJECT_TYPE MATCHES Custom)
		if (NOT ${PROJECT_NAME}_SRCS)
			message(FATAL_ERROR "no sources")
		endif (NOT ${PROJECT_NAME}_SRCS)
	endif (NOT ${PROJECT_NAME}_PROJECT_TYPE MATCHES Custom)

	ow_add_public_libraries(${PROJECT_NAME})

	if (${PROJECT_NAME}_PROJECT_TYPE MATCHES Shared OR ${PROJECT_NAME}_PROJECT_TYPE MATCHES Plugin)
		ow_add_shared_library_definitions()
	endif (${PROJECT_NAME}_PROJECT_TYPE MATCHES Shared OR ${PROJECT_NAME}_PROJECT_TYPE MATCHES Plugin)

	ow_unique(unique ${${PROJECT_NAME}_DEFINITIONS} ${${PROJECT_NAME}_PRIVATE_DEFINITIONS})
	add_definitions(${unique})

	ow_unique(unique ${${PROJECT_NAME}_INCLUDE_DIRS} ${${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS})
	include_directories(${unique})

	ow_unique(unique ${${PROJECT_NAME}_LIBRARY_DIRS} ${${PROJECT_NAME}_PRIVATE_LIBRARY_DIRS})
	link_directories(${unique})

endmacro (OW_PREPARE_BINARY)
