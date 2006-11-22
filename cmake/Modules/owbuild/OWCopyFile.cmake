# - OW_COPY_FILE(src dst)
# Copies a file to ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${dst}
# only if src is different (newer) than dst
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_COPY_FILE src dst)

	string(TOLOWER ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE} dir)

	# Removes all path containing .svn or CVS or CMakeLists.txt during the copy
	if (NOT ${src} MATCHES ".*\\.svn|CVS|CMakeLists\\.txt.*")

		message(STATUS "Copy file from ${src} to ${dir}/${dst}")

		exec_program(${CMAKE_COMMAND}
			ARGS
				"-E copy_if_different \"${src}\" \"${dir}/${dst}\""
		)
	endif (NOT ${src} MATCHES ".*\\.svn|CVS|CMakeLists\\.txt.*")

endmacro (OW_COPY_FILE)
