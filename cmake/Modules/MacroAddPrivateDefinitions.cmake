# - MACRO_ADD_PRIVATE_DEFINITIONS(def1 ... defN)

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_ADD_PRIVATE_DEFINITIONS)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

	add_definitions(
		${ARGN}
	)

endmacro (MACRO_ADD_PRIVATE_DEFINITIONS)
