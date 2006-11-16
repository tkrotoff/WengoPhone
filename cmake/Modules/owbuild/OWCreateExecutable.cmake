# - OW_CREATE_EXECUTABLE(name)
# Creates an executable (.exe)
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_EXECUTABLE name)

	ow_create_project(${name})

	set(${PROJECT_NAME}_PROJECT_TYPE
		Executable
		CACHE STRING "${PROJECT_NAME} project type (Static, Shared, Plugin, Executable, Custom)"
	)

endmacro (OW_CREATE_EXECUTABLE)
