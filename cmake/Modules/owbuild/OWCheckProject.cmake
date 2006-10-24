# - OW_CHECK_PROJECT()
# Checks if the project is valid or not
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CHECK_PROJECT)

	if (NOT PROJECT_NAME)
		message(FATAL_ERROR "no project name defined")
	endif (NOT PROJECT_NAME)

endmacro (OW_CHECK_PROJECT)
