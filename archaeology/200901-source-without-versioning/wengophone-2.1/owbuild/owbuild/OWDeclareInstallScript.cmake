# - ow_declare_install_script()
# Declares a CMakeLists.txt that will be run when ${PROJECT_NAME}-install target is run
#
# Permits to have not only one install target but many ${PROJECT_NAME}-install targets:
# allows more flexibility
# Creates directory ${CMAKE_BINARY_DIR}/install/${PROJECT_NAME} and copies generated files
# from install target inside
#
# Copyright (C) 2006-2007  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (ow_declare_install_script)

	ow_check_project()

	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/install/${PROJECT_NAME}")

	set(_MORE_OPTIONS "")
	if (WIN32)
		set(_MORE_OPTIONS "-G" "NMake Makefiles")
	endif (WIN32)

	add_custom_target(${PROJECT_NAME}-install
		"cmake" ${_MORE_OPTIONS} "${CMAKE_CURRENT_SOURCE_DIR}/install" "-DSOURCE_DIR=${CMAKE_SOURCE_DIR}" "-DBINARY_DIR=${CMAKE_BINARY_DIR}"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/install/${PROJECT_NAME}")

	add_dependencies(${PROJECT_NAME}-install ${PROJECT_NAME})

endmacro (ow_declare_install_script)
