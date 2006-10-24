# - OW_ADD_PRIVATE_LIBRARIES(lib1 ... libN)
# Adds private link libraries to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_PRIVATE_LIBRARIES)

	ow_check_project()

	if (${PROJECT_NAME}_PRIVATE_LIBRARIES)
		set(${PROJECT_NAME}_PRIVATE_LIBRARIES
			${${PROJECT_NAME}_PRIVATE_LIBRARIES}
			${ARGN}
		)
	else (${PROJECT_NAME}_PRIVATE_LIBRARIES)
		set(${PROJECT_NAME}_PRIVATE_LIBRARIES
			${ARGN}
		)
	endif (${PROJECT_NAME}_PRIVATE_LIBRARIES)

endmacro (OW_ADD_PRIVATE_LIBRARIES)
