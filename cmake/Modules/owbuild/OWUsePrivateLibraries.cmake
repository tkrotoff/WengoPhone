# - OW_USE_PRIVATE_LIBRARIES(lib1 ... libN)
# Uses privately a library inside the current project: imports properties from a library to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_USE_PRIVATE_LIBRARIES)

	ow_check_project()

	foreach (loop ${ARGN})
		if (NOT ${loop}_INCLUDE_DIRS)
			message(FATAL_ERROR "${loop}_INCLUDE_DIRS empty")
		endif (NOT ${loop}_INCLUDE_DIRS)
		ow_add_private_include_dirs(
			${${loop}_INCLUDE_DIRS}
		)

		if (NOT ${loop}_LIBRARIES)
			message(FATAL_ERROR "${loop}_LIBRARIES empty")
		endif (NOT ${loop}_LIBRARIES)
		ow_add_private_libraries(
			${${loop}_LIBRARIES}
		)

		if (${loop}_DEFINITIONS)
			ow_add_private_definitions(
				${${loop}_DEFINITIONS}
			)
		endif (${loop}_DEFINITIONS)
	endforeach (loop)

endmacro (OW_USE_PRIVATE_LIBRARIES)
