# - OW_CREATE_SHARED_LIBRARY(name)
# Creates a shared library (.dll, .dylib, .so)
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_SHARED_LIBRARY name)

	ow_create_project(${name})

	set(${PROJECT_NAME}_PROJECT_TYPE
		Shared
		CACHE STRING "${PROJECT_NAME} project type (Static, Shared, Plugin, Executable)"
	)

endmacro (OW_CREATE_SHARED_LIBRARY)
