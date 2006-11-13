# - OW_POST_BUILD_COPY_FILE(src dst)
# Copies a file (foo.lib, foo.a, foo.exe, foo.dylib...) to ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE} using the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_POST_BUILD_COPY_FILE src dst)

	ow_check_project()

	string(TOLOWER ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE} dir)

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			${src}
			${dir}/${dst}
	)

endmacro (OW_POST_BUILD_COPY_FILE)
