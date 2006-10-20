# - MACRO_CREATE_STATIC_LIBRARY()

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_CREATE_STATIC_LIBRARY)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

	if (NOT ${PROJECT_NAME}_SRCS)
		message(FATAL_ERROR "no sources")
	endif (NOT ${PROJECT_NAME}_SRCS)

	MACRO_ADD_LIBRARIES(${PROJECT_NAME})

	add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRCS})

	target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LINK_LIBRARIES})

	MACRO_PROJECT_LOG()

endmacro (MACRO_CREATE_STATIC_LIBRARY)
