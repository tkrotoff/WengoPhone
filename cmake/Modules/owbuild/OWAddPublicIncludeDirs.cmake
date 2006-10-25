# - OW_ADD_PUBLIC_INCLUDE_DIRS(dir1 ... dirN)
# Adds public include directories to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PUBLIC_INCLUDE_DIRS)

	ow_check_project()

	if (${PROJECT_NAME}_INCLUDE_DIRS)
		set(${PROJECT_NAME}_INCLUDE_DIRS
			${${PROJECT_NAME}_INCLUDE_DIRS}
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public include directories"
		)
	else (${PROJECT_NAME}_INCLUDE_DIRS)
		set(${PROJECT_NAME}_INCLUDE_DIRS
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public include directories"
		)
	endif (${PROJECT_NAME}_INCLUDE_DIRS)

	ow_unique(unique ${${PROJECT_NAME}_INCLUDE_DIRS})
	set(${PROJECT_NAME}_INCLUDE_DIRS
		${unique}
		CACHE INTERNAL "${PROJECT_NAME} public include directories"
	)

endmacro (OW_ADD_PUBLIC_INCLUDE_DIRS)
