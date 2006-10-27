# - OW_ADD_PRIVATE_INCLUDE_DIRS(dir1 ... dirN)
# Adds private include directories to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PRIVATE_INCLUDE_DIRS)

	ow_check_project()

	if (${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS)
		set(${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS
			${${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS}
			${ARGN}
		)
	else (${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS)
		set(${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS
			${ARGN}
		)
	endif (${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS)

endmacro (OW_ADD_PRIVATE_INCLUDE_DIRS)
