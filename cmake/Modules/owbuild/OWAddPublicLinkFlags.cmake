# - OW_ADD_PUBLIC_LINK_FLAGS(flag1 ... flagN)
# Adds public link flags to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PUBLIC_LINK_FLAGS)

	ow_check_project()

	if (${PROJECT_NAME}_PUBLIC_LINK_FLAGS)
		set(${PROJECT_NAME}_PUBLIC_LINK_FLAGS
			${${PROJECT_NAME}_PUBLIC_LINK_FLAGS}
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public link flags"
		)
	else (${PROJECT_NAME}_PUBLIC_LINK_FLAGS)
		set(${PROJECT_NAME}_PUBLIC_LINK_FLAGS
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} public link flags"
		)
	endif (${PROJECT_NAME}_PUBLIC_LINK_FLAGS)

	ow_unique(unique ${${PROJECT_NAME}_PUBLIC_LINK_FLAGS})
	set(${PROJECT_NAME}_PUBLIC_LINK_FLAGS
		${unique}
		CACHE INTERNAL "${PROJECT_NAME} public link flags"
	)

endmacro (OW_ADD_PUBLIC_LINK_FLAGS)
