# - MACRO_ADD_DEFINITIONS(def1 ... defN)

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_ADD_DEFINITIONS)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

	if (${PROJECT_NAME}_DEFINITIONS)
		set(${PROJECT_NAME}_DEFINITIONS
			${${PROJECT_NAME}_DEFINITIONS}
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} definitions"
		)
	else (${PROJECT_NAME}_DEFINITIONS)
		set(${PROJECT_NAME}_DEFINITIONS
			${ARGN}
			CACHE INTERNAL "${PROJECT_NAME} definitions"
		)
	endif (${PROJECT_NAME}_DEFINITIONS)

endmacro (MACRO_ADD_DEFINITIONS)
