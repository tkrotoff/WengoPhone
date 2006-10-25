# - OW_ADD_PUBLIC_LIBRARIES(lib1 ... libN)
# Adds public link libraries to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PUBLIC_LIBRARIES)

	ow_check_project()

	if (${PROJECT_NAME}_LIBRARIES)
		set(${PROJECT_NAME}_LIBRARIES
			${${PROJECT_NAME}_LIBRARIES}
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public libraries"
		)
	else (${PROJECT_NAME}_LIBRARIES)
		set(${PROJECT_NAME}_LIBRARIES
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public libraries"
		)
	endif (${PROJECT_NAME}_LIBRARIES)

	ow_unique(unique ${${PROJECT_NAME}_LIBRARIES})
	set(${PROJECT_NAME}_LIBRARIES
		${unique}
		CACHE INTERNAL "${PROJECT_NAME} public libraries"
	)

endmacro (OW_ADD_PUBLIC_LIBRARIES)
