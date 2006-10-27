# - OW_ADD_PRIVATE_COMPILE_FLAGS(flag1 ... flagN)
# Adds private compilation flags to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PRIVATE_COMPILE_FLAGS)

	ow_check_project()

	if (${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS)
		set(${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS
			${${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS}
			${ARGN}
		)
	else (${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS)
		set(${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS
			${ARGN}
		)
	endif (${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS)

endmacro (OW_ADD_PRIVATE_COMPILE_FLAGS)
