# - OW_CREATE_PROJECT(name)
# Creates an empty project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_PROJECT name)

	project(${name})

	set(${PROJECT_NAME}_SRCS "")
	set(${PROJECT_NAME}_PUBLIC_INCLUDE_DIRS "" CACHE INTERNAL "${PROJECT_NAME} public include directories")
	set(${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS "")
	set(${PROJECT_NAME}_PUBLIC_LIBRARIES "" CACHE INTERNAL "${PROJECT_NAME} public libraries")
	set(${PROJECT_NAME}_PRIVATE_LIBRARIES "")
	set(${PROJECT_NAME}_PUBLIC_DEFINITIONS "" CACHE INTERNAL "${PROJECT_NAME} public definitions")
	set(${PROJECT_NAME}_PRIVATE_DEFINITIONS "")
	set(${PROJECT_NAME}_PRIVATE_LINK_FLAGS "")
	set(${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS "")

endmacro (OW_CREATE_PROJECT)
