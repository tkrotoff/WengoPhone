# - OW_CREATE_PROJECT(name)
# Creates an empty project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_PROJECT name)

	# Creates the variable ${PROJECT_NAME} containing the project name
	project(${name})

	set(${PROJECT_NAME}_SRCS "")

	# Resets the include directories
	set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "")
	set(${PROJECT_NAME}_INCLUDE_DIRS "" CACHE INTERNAL "${PROJECT_NAME} public include directories")
	set(${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS "")

	set(${PROJECT_NAME}_LIBRARIES "" CACHE INTERNAL "${PROJECT_NAME} public libraries")
	set(${PROJECT_NAME}_PRIVATE_LIBRARIES "")

	# Resets link directories (e.g library directories, e.g library paths)
	set_directory_properties(PROPERTIES LINK_DIRECTORIES "")
	set(${PROJECT_NAME}_LIBRARY_DIRS "" CACHE INTERNAL "${PROJECT_NAME} public library directories")
	set(${PROJECT_NAME}_PRIVATE_LIBRARY_DIRS "" CACHE INTERNAL "${PROJECT_NAME} private library directories")

	# Resets the definitions
	set_directory_properties(PROPERTIES DEFINITIONS "")
	set_directory_properties(PROPERTIES DEFINITION "")
	set(${PROJECT_NAME}_DEFINITIONS "" CACHE INTERNAL "${PROJECT_NAME} public definitions")
	set(${PROJECT_NAME}_PRIVATE_DEFINITIONS "")

	# Resets compile flags
	set(${PROJECT_NAME}_PRIVATE_COMPILE_FLAGS "")

	# Resets link flags
	set(${PROJECT_NAME}_PRIVATE_LINK_FLAGS "")

	# Resets build version and API version
	set(${PROJECT_NAME}_BUILD_VERSION "")
	set(${PROJECT_NAME}_API_VERSION "")

endmacro (OW_CREATE_PROJECT)
