# - OW_ADD_PUBLIC_LIBRARIES(lib1 ... libN)
# Adds public link libraries to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PUBLIC_LIBRARIES)

	ow_check_project()

	if (${PROJECT_NAME}_PUBLIC_LIBRARIES)
		set(${PROJECT_NAME}_PUBLIC_LIBRARIES
			${${PROJECT_NAME}_PUBLIC_LIBRARIES}
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} libraries"
		)
	else (${PROJECT_NAME}_PUBLIC_LIBRARIES)
		set(${PROJECT_NAME}_PUBLIC_LIBRARIES
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} libraries"
		)
	endif (${PROJECT_NAME}_PUBLIC_LIBRARIES)

	ow_unique(unique ${${PROJECT_NAME}_PUBLIC_LIBRARIES})
	set(${PROJECT_NAME}_PUBLIC_LIBRARIES
		${unique}
		CACHE INTERNAL "${PROJECT_NAME} libraries"
	)

endmacro (OW_ADD_PUBLIC_LIBRARIES)
