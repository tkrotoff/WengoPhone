# - ow_create_static_library(name)
# Creates a static library (.lib, .a) given its name
#
# ow_create_static_library() creates a project named ${name} (${PROJECT_NAME})
# and works in combination with ow_create_project_binary()
#
# Example:
# ow_create_static_library(mytest)
#
# ow_add_sources(
# 	mytest.c
# )
#
# ow_create_project_binary()
#
# Output generated:
# gcc mytext.c -static -o mytest.so
#
# Internally, ow_create_static_library() sets variable ${PROJECT_NAME}_PROJECT_TYPE
# to Static
# This variable will be used later on by ow_create_project_binary()
#
# Copyright (C) 2006-2007  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (ow_create_static_library name)

	ow_create_project(${name})

	set(${PROJECT_NAME}_PROJECT_TYPE
		Static
		CACHE STRING "${PROJECT_NAME} project type (Static, Shared, Plugin, Executable)"
	)

endmacro (ow_create_static_library)
