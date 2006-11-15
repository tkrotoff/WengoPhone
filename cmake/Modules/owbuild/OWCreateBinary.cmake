# - OW_CREATE_BINARY()
# Creates a binary (static/shared/plugin library or a executable) using the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_BINARY)

	ow_prepare_binary()

	if (${PROJECT_NAME}_PROJECT_TYPE MATCHES Static)
		add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRCS})

		if (WIN32)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.lib ${PROJECT_NAME}.lib)
		else (WIN32)

		if (UNIX)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.a lib${PROJECT_NAME}.a)
		endif (UNIX)

		endif (WIN32)
	else (${PROJECT_NAME}_PROJECT_TYPE MATCHES Static)

	if (${PROJECT_NAME}_PROJECT_TYPE MATCHES Shared)
		add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRCS})

		if (WIN32)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.lib ${PROJECT_NAME}.lib)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.dll ${PROJECT_NAME}.dll)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdb ${PROJECT_NAME}.pdb)
		else (WIN32)

		if (APPLE)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.dylib lib${PROJECT_NAME}.dylib)
		else (APPLE)

		if (UNIX)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.so lib${PROJECT_NAME}.so)
		endif (UNIX)

		endif (WIN32)
		endif (APPLE)
	else (${PROJECT_NAME}_PROJECT_TYPE MATCHES Shared)

	if (${PROJECT_NAME}_PROJECT_TYPE MATCHES Plugin)
		list(REMOVE_ITEM ${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME})
		set(${PROJECT_NAME}_LIBRARIES
			${${PROJECT_NAME}_LIBRARIES}
			CACHE INTERNAL "${PROJECT_NAME} public libraries"
		)

		add_library(${PROJECT_NAME} MODULE ${${PROJECT_NAME}_SRCS})

		if (WIN32)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.dll ${PROJECT_NAME}.dll)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdb ${PROJECT_NAME}.pdb)
		else (WIN32)

		if (APPLE)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.dylib lib${PROJECT_NAME}.dylib)
		else (APPLE)

		if (UNIX)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.so lib${PROJECT_NAME}.so)
		endif (UNIX)

		endif (WIN32)
		endif (APPLE)
	else (${PROJECT_NAME}_PROJECT_TYPE MATCHES Plugin)

	if (${PROJECT_NAME}_PROJECT_TYPE MATCHES Executable)
		list(REMOVE_ITEM ${PROJECT_NAME}_LIBRARIES ${PROJECT_NAME})
		set(${PROJECT_NAME}_LIBRARIES
			${${PROJECT_NAME}_LIBRARIES}
			CACHE INTERNAL "${PROJECT_NAME} public libraries"
		)

		add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})

		if (WIN32)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe ${PROJECT_NAME}.exe)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pdb ${PROJECT_NAME}.pdb)
		else (WIN32)

		if (UNIX AND NOT APPLE)
			ow_post_build_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${PROJECT_NAME})
		endif (UNIX AND NOT APPLE)

		endif (WIN32)
	endif (${PROJECT_NAME}_PROJECT_TYPE MATCHES Executable)

	endif (${PROJECT_NAME}_PROJECT_TYPE MATCHES Plugin)
	endif (${PROJECT_NAME}_PROJECT_TYPE MATCHES Shared)
	endif (${PROJECT_NAME}_PROJECT_TYPE MATCHES Static)

	ow_finish_binary()

endmacro (OW_CREATE_BINARY)
