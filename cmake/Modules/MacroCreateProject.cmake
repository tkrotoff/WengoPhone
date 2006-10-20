# - MACRO_CREATE_PROJECT(name)

# Copyright (c) 2006, Tanguy Krotoff <tanguy.krotoff@wengo.fr>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


macro (MACRO_CREATE_PROJECT name)

	project(${name})

	set(${PROJECT_NAME}_SRCS "")
	set(${PROJECT_NAME}_INCLUDE_DIRS "" CACHE INTERNAL "${PROJECT_NAME} include directories")
	set(${PROJECT_NAME}_LINK_LIBRARIES "")
	set(${PROJECT_NAME}_LIBRARIES "" CACHE INTERNAL "${PROJECT_NAME} libraries")
	set(${PROJECT_NAME}_DEFINITIONS "" CACHE INTERNAL "${PROJECT_NAME} definitions")

endmacro (MACRO_CREATE_PROJECT)
