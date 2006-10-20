# - MACRO_PROJECT_LOG()

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_PROJECT_LOG)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

	message(STATUS "**")
	message(STATUS "** PROJECT_NAME=${PROJECT_NAME}")
	message(STATUS "** ${PROJECT_NAME}_SRCS=${${PROJECT_NAME}_SRCS}")
	message(STATUS "** ${PROJECT_NAME}_INCLUDE_DIRS=${${PROJECT_NAME}_INCLUDE_DIRS}")
	message(STATUS "** ${PROJECT_NAME}_LINK_LIBRARIES=${${PROJECT_NAME}_LINK_LIBRARIES}")
	message(STATUS "** ${PROJECT_NAME}_LIBRARIES=${${PROJECT_NAME}_LIBRARIES}")
	message(STATUS "** ${PROJECT_NAME}_DEFINITIONS=${${PROJECT_NAME}_DEFINITIONS}")
	message(STATUS "**")

endmacro (MACRO_PROJECT_LOG)
