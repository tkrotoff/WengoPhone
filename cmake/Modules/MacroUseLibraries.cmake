# - MACRO_USE_LIBRARIES(lib1 ... libN)

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_USE_LIBRARIES)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

	foreach(loop ${ARGN})
		if (NOT ${loop}_INCLUDE_DIRS)
			message(FATAL_ERROR "${loop}_INCLUDE_DIRS empty")
		endif (NOT ${loop}_INCLUDE_DIRS)
		MACRO_ADD_INCLUDE_DIRS(
			${${loop}_INCLUDE_DIRS}
		)

		if (NOT ${loop}_LIBRARIES)
			message(FATAL_ERROR "${loop}_LIBRARIES empty")
		endif (NOT ${loop}_LIBRARIES)
		MACRO_ADD_LINK_LIBRARIES(
			${${loop}_LIBRARIES}
		)

		if (${loop}_DEFINITIONS)
			MACRO_ADD_PRIVATE_DEFINITIONS(
				${${loop}_DEFINITIONS}
			)
		endif (${loop}_DEFINITIONS)
	endforeach(loop)

endmacro (MACRO_USE_LIBRARIES)
