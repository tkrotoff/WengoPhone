# - OW_GET_SUBVERSION_REVISION(revision)
# Gets the current subversion revision number
#
# Copyright (C) 2006  Andreas Schneider <mail@cynapses.org>
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_GET_SUBVERSION_REVISION revision)
	find_program(SVN_EXECUTEABLE
		NAMES
			svn
		PATHS
			/usr/bin
			/usr/local/bin
	)

	find_file(SVN_DOT_DIR
		NAMES
			entries
		PATHS
			${CMAKE_SOURCE_DIR}/.svn
	)

	if (SVN_EXECUTEABLE AND SVN_DOT_DIR)
		execute_process(
			COMMAND
				svnversion -n ${CMAKE_SOURCE_DIR}/cmake
			RESULT_VARIABLE
				SVN_REVISION_RESULT_VARIABLE
			OUTPUT_VARIABLE
				SVN_REVISION_OUTPUT_VARIABLE
		)

		if (SVN_REVISION_RESULT_VARIABLE EQUAL 0)
			set(SVN_REVISION
				${SVN_REVISION_OUTPUT_VARIABLE}
			)
		else (SVN_REVISION_RESULT_VARIABLE EQUAL 0)
			set(SVN_REVISION
				0
			)
		endif (SVN_REVISION_RESULT_VARIABLE EQUAL 0)
		message(STATUS "svn revision: " ${SVN_REVISION})
	endif (SVN_EXECUTEABLE AND SVN_DOT_DIR)

	set(${revision}
		${SVN_REVISION}
		CACHE INTERNAL "svn revision"
	)
endmacro (OW_GET_SUBVERSION_REVISION revision)
