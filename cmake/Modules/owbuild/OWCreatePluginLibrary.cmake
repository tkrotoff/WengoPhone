# - OW_CREATE_PLUGIN_LIBRARY()
# Creates a plugin library (e.g a shared library) using the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_PLUGIN_LIBRARY)

	ow_prepare_binary()

	add_library(${PROJECT_NAME} MODULE ${${PROJECT_NAME}_SRCS})

	target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_PRIVATE_LIBRARIES})

	install(TARGETS ${PROJECT_NAME} DESTINATION bin/)

endmacro (OW_CREATE_PLUGIN_LIBRARY)
