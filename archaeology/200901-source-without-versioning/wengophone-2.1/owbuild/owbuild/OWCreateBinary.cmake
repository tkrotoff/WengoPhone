# - ow_create_binary(arg1 ... argN)
# Creates a binary (static/shared/plugin library or a executable) using the current project
#
# This function should be the last one to be called in a CMakeLists.txt
# The binary generated is named after ${PROJECT_NAME} variable
# The type of binary generated depends on the function ow_create_*() that you called before:
# - ow_create_executable()
# - ow_create_shared_library()
# - ow_create_static_library()
# - ow_create_plugin_library()
# This info is stored inside ${PROJECT_NAME}_PROJECT_TYPE variable
# that can be either Static, Shared, Plugin or Executable
# ${PROJECT_NAME}_PROJECT_TYPE variable can be overwritten via CMake cache
# This allow you to change a static library to a shared one for example
# even if ow_create_static_library() is used inside the CMakeLists.txt
#
# If you used ow_create_executable(), arguments can be:
# WIN32, MACOSX_BUNDLE, EXCLUDE_FROM_ALL...
# Check CMake documentation for full documentation
#
# When generated, binaries are automatically copied to
# ${BUILD_DIR} directory using ow_post_build_copy_file()
# Not all binaries are copied, only *.pdb, *.exe, *.so, *.dylib, *.dll, *.app
# .a and .lib binaries are not copied (not useful to have them inside ${BUILD_DIR}
#
# Example:
# ow_create_executable(mytest)
#
# ow_add_sources(
# 	mytest.c
# )
#
# ow_create_binary()
#
# Output generated:
# gcc mytext.c -o mytest
#
# Internally ow_create_binary() wrapps CMake functions add_library() and add_executable()
# ow_create_binary() calls ow_prepare_binary() first, performs some tasks and then calls
# ow_finish_binary() in order to complete the binary generation processus
#
# Copyright (C) 2006-2007  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (ow_create_binary)

	ow_prepare_binary()

	if (${PROJECT_NAME}_PROJECT_TYPE STREQUAL Static)
		add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRCS})
	endif (${PROJECT_NAME}_PROJECT_TYPE STREQUAL Static)

	# Shared library
	if (${PROJECT_NAME}_PROJECT_TYPE STREQUAL Shared)
		add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRCS})

		if (MSVC)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.dll .)
			if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
				ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdb .)
			endif (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
		endif (MSVC)

		if (MINGW)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.dll ./${PROJECT_NAME}.dll)
		endif (MINGW)

		if (UNIX AND NOT APPLE)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.so .)
		endif (UNIX AND NOT APPLE)
	endif (${PROJECT_NAME}_PROJECT_TYPE STREQUAL Shared)

	# Plugin library
	if (${PROJECT_NAME}_PROJECT_TYPE STREQUAL Plugin)
		list(REMOVE_ITEM ${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME})
		set(${PROJECT_NAME}_LIBRARIES
			${${PROJECT_NAME}_LIBRARIES}
			CACHE INTERNAL "${PROJECT_NAME} public libraries"
		)

		add_library(${PROJECT_NAME} MODULE ${${PROJECT_NAME}_SRCS})

		if (MSVC)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.dll .)
			if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
				ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdb .)
			endif (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
		endif (MSVC)

		if (MINGW)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.dll ./lib${PROJECT_NAME}.dll)
		endif (MINGW)

		if (UNIX AND NOT APPLE)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.so .)
		endif (UNIX AND NOT APPLE)
	endif (${PROJECT_NAME}_PROJECT_TYPE STREQUAL Plugin)

	# Executable
	if (${PROJECT_NAME}_PROJECT_TYPE STREQUAL Executable)
		list(REMOVE_ITEM ${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME})
		set(${PROJECT_NAME}_LIBRARIES
			${${PROJECT_NAME}_LIBRARIES}
			CACHE INTERNAL "${PROJECT_NAME} public libraries"
		)

		if (CMAKE_BUILD_TYPE STREQUAL Debug)
			# With console, /SUBSYSTEM:CONSOLE
			add_executable(${PROJECT_NAME} ${ARGN} ${${PROJECT_NAME}_SRCS})
		else (CMAKE_BUILD_TYPE STREQUAL Debug)
			# Without console, /SUBSYSTEM:WINDOWS
			add_executable(${PROJECT_NAME} WIN32 ${ARGN} ${${PROJECT_NAME}_SRCS})
		endif (CMAKE_BUILD_TYPE STREQUAL Debug)

		if (MSVC)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe .)
			if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
				ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdb .)
			endif (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
		endif (MSVC)

		if (APPLE)
			ow_list_contains("${ARGN}" "MACOSX_BUNDLE" result)
			if (result)
				ow_post_build_copy_dir(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app ${PROJECT_NAME}.app)
			else (result)
				ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} .)
			endif (result)
		endif (APPLE)

		if (UNIX AND NOT APPLE)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} .)
		endif (UNIX AND NOT APPLE)
	endif (${PROJECT_NAME}_PROJECT_TYPE STREQUAL Executable)

	ow_finish_binary()

endmacro (ow_create_binary)
