# - MACRO_ADD_SOURCES(src1 ... srcN)

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_ADD_SOURCES)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

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

endmacro (MACRO_ADD_SOURCES)
