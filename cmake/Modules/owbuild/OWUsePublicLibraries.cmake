# - OW_USE_PUBLIC_LIBRARIES(lib1 ... libN)
# Uses publicly a library inside the current project: imports properties from a library to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_USE_PUBLIC_LIBRARIES)

	ow_check_project()

	foreach (loop ${ARGN})
		if (NOT ${loop}_INCLUDE_DIRS)
			if (NOT ${loop}_INCLUDE_DIR)
				message(FATAL_ERROR "${PROJECT_NAME}: ${loop}_INCLUDE_DIRS and ${loop}_INCLUDE_DIR empty")
			endif (NOT ${loop}_INCLUDE_DIR)
		endif (NOT ${loop}_INCLUDE_DIRS)
		ow_add_public_include_dirs(
			${${loop}_INCLUDE_DIRS}
			${${loop}_INCLUDE_DIR}
		)

		#if (NOT ${loop}_LIBRARIES)
		#	message(FATAL_ERROR "${loop}_LIBRARIES empty")
		#endif (NOT ${loop}_LIBRARIES)
		if (${loop}_LIBRARIES)
			ow_add_public_libraries(
				${${loop}_LIBRARIES}
			)
		endif (${loop}_LIBRARIES)

		if (${loop}_LIBRARY_DIRS)
			ow_add_public_library_dirs(
				${${loop}_LIBRARY_DIRS}
			)
		endif (${loop}_LIBRARY_DIRS)

		if (${loop}_DEFINITIONS)
			ow_add_public_definitions(
				${${loop}_DEFINITIONS}
			)
		endif (${loop}_DEFINITIONS)

		if (${loop}_PUBLIC_LINK_FLAGS)
			ow_add_public_link_flags(
				${${loop}_PUBLIC_LINK_FLAGS}
			)
		endif (${loop}_PUBLIC_LINK_FLAGS)
	endforeach (loop)

endmacro (OW_USE_PUBLIC_LIBRARIES)
