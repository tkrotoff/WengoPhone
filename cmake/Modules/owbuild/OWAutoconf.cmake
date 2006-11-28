# - OW_AUTOCONF(configureCommand makeCommand)
# Runs autoconf tool.
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_AUTOCONF configureCommand makeCommand)

	execute_process(
		COMMAND
			${configureCommand}
		WORKING_DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}
	)

	execute_process(
		COMMAND
			${makeCommand}
		WORKING_DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}
	)

endmacro (OW_AUTOCONF)
