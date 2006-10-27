# - OW_ADD_PRIVATE_DEFINITIONS(def1 ... defN)
# Adds private defines to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PRIVATE_DEFINITIONS)

	ow_check_project()

	if (${PROJECT_NAME}_PRIVATE_DEFINITIONS)
		set(${PROJECT_NAME}_PRIVATE_DEFINITIONS
			${${PROJECT_NAME}_PRIVATE_DEFINITIONS}
			${ARGN}
		)
	else (${PROJECT_NAME}_PRIVATE_DEFINITIONS)
		set(${PROJECT_NAME}_PRIVATE_DEFINITIONS
			${ARGN}
		)
	endif (${PROJECT_NAME}_PRIVATE_DEFINITIONS)

endmacro (OW_ADD_PRIVATE_DEFINITIONS)
