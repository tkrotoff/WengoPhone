# - MACRO_ADD_INCLUDE_DIRS(dir1 ... dirN)

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_ADD_INCLUDE_DIRS)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

	if (${PROJECT_NAME}_INCLUDE_DIRS)
		set(${PROJECT_NAME}_INCLUDE_DIRS
			${${PROJECT_NAME}_INCLUDE_DIRS}
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} include directories"
		)
	else (${PROJECT_NAME}_INCLUDE_DIRS)
		set(${PROJECT_NAME}_INCLUDE_DIRS
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} include directories"
		)
	endif (${PROJECT_NAME}_INCLUDE_DIRS)

	include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

endmacro (MACRO_ADD_INCLUDE_DIRS)
