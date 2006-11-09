# - OW_ADD_PUBLIC_LIBRARY_DIRS(dir1 ... dirN)
# Adds public link directories (directories in which to search for libraries
# e.g library path) to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PUBLIC_LIBRARY_DIRS)

	ow_check_project()

	if (${PROJECT_NAME}_LIBRARY_DIRS)
		set(${PROJECT_NAME}_LIBRARY_DIRS
			${${PROJECT_NAME}_LIBRARY_DIRS}
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public library directories"
		)
	else (${PROJECT_NAME}_LIBRARY_DIRS)
		set(${PROJECT_NAME}_LIBRARY_DIRS
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public library directories"
		)
	endif (${PROJECT_NAME}_LIBRARY_DIRS)

	ow_unique(unique ${${PROJECT_NAME}_LIBRARY_DIRS})
	set(${PROJECT_NAME}_LIBRARY_DIRS
		${unique}
		CACHE INTERNAL "${PROJECT_NAME} public library directories"
	)

endmacro (OW_ADD_PUBLIC_LIBRARY_DIRS)
