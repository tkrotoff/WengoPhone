# - OW_CHECK_PROJECT()
# Internal function, checks if the project is valid or not
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CHECK_PROJECT)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "No project name defined, "
			"use ow_create_executable() or ow_create_plugin_library() or "
			"ow_create_shared_library() or ow_create_static_library() or "
			"ow_create_project() first")
	endif (NOT PROJECT_NAME)

endmacro (OW_CHECK_PROJECT)
