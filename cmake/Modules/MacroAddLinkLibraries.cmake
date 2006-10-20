# - MACRO_ADD_LINK_LIBRARIES(lib1 ... libN)

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_ADD_LINK_LIBRARIES)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

	if (${PROJECT_NAME}_LINK_LIBRARIES)
		set(${PROJECT_NAME}_LINK_LIBRARIES
			${${PROJECT_NAME}_LINK_LIBRARIES}
			${ARGN}
		)
	else (${PROJECT_NAME}_LINK_LIBRARIES)
		set(${PROJECT_NAME}_LINK_LIBRARIES
			${ARGN}
		)
	endif (${PROJECT_NAME}_LINK_LIBRARIES)

endmacro (MACRO_ADD_LINK_LIBRARIES)
