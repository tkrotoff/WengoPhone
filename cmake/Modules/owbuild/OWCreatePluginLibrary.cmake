# - OW_CREATE_PLUGIN_LIBRARY()
# Creates a plugin library (e.g a shared library) using the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_PLUGIN_LIBRARY)

	ow_prepare_binary()

	list(REMOVE_ITEM ${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME})
	set(${PROJECT_NAME}_LIBRARIES
		${${PROJECT_NAME}_LIBRARIES}
		CACHE INTERNAL "${PROJECT_NAME} public libraries"
	)

	add_library(${PROJECT_NAME} MODULE ${${PROJECT_NAME}_SRCS})

	ow_finish_binary()

endmacro (OW_CREATE_PLUGIN_LIBRARY)
