# - MACRO_ENSURE_OUT_OF_SOURCE_BUILD(errorMessage)
#
# Copyright (C) 2006  Alexander Neundorf <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (MACRO_ENSURE_OUT_OF_SOURCE_BUILD errorMessage)

	string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource)
	if (insource)
		message(FATAL_ERROR "${errorMessage}")
	endif (insource)

endmacro (MACRO_ENSURE_OUT_OF_SOURCE_BUILD)
