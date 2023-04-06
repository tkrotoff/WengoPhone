# - ow_post_build_copy_dir(src dst)
# Internal function, copies a directory to ${BUILD_DIR} using the current project
#
# The copy command is run after target ${PROJECT_NAME} has been built (POST_BUILD option from CMake)
# Works the same as ow_copy_dir()
#
# Example:
# ow_create_executable(mytest)
#
# ow_add_sources(
# 	mytest.c
# )
#
# ow_post_build_copy_dir(${CMAKE_CURRENT_SOURCE_DIR}/*.png .)
#
# ow_create_binary(mytest)
#
# Copyright (C) 2006-2007  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (ow_post_build_copy_dir src dst)

	ow_check_project()

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			\"${src}\"
			\"${BUILD_DIR}/${dst}\"
		COMMENT "Copying dir ${dst}"
	)

endmacro (ow_post_build_copy_dir)
