# - OW_ADD_PUBLIC_DEFINITIONS(def1 ... defN)
# Adds public defines to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PUBLIC_DEFINITIONS)

	ow_check_project()

	if (${PROJECT_NAME}_DEFINITIONS)
		set(${PROJECT_NAME}_DEFINITIONS
			${${PROJECT_NAME}_DEFINITIONS}
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public definitions"
		)
	else (${PROJECT_NAME}_DEFINITIONS)
		set(${PROJECT_NAME}_DEFINITIONS
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public definitions"
		)
	endif (${PROJECT_NAME}_DEFINITIONS)

	ow_unique(unique ${${PROJECT_NAME}_DEFINITIONS})
	set(${PROJECT_NAME}_DEFINITIONS
		${unique}
		CACHE INTERNAL "${PROJECT_NAME} public definitions"
	)

endmacro (OW_ADD_PUBLIC_DEFINITIONS)
