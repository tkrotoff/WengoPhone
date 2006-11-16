# - OW_CREATE_PLUGIN_LIBRARY(name)
# Creates a plugin library (e.g a shared library)
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_PLUGIN_LIBRARY name)

	ow_create_project(${name})

	set(${PROJECT_NAME}_PROJECT_TYPE
		Plugin
		CACHE STRING "${PROJECT_NAME} project type (Static, Shared, Plugin, Executable)"
	)

endmacro (OW_CREATE_PLUGIN_LIBRARY)
