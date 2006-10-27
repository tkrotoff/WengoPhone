# - OW_ADD_SOURCES(src1 ... srcN)
# Adds source files (.cpp, .c) to the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_SOURCES)

	ow_check_project()

	if (${PROJECT_NAME}_SRCS)
		set(${PROJECT_NAME}_SRCS
			${${PROJECT_NAME}_SRCS}
			${ARGN}
		)
	else (${PROJECT_NAME}_SRCS)
		set(${PROJECT_NAME}_SRCS
			${ARGN}
		)
	endif (${PROJECT_NAME}_SRCS)

endmacro (OW_ADD_SOURCES)
