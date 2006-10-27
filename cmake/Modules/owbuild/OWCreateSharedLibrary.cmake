# - OW_CREATE_SHARED_LIBRARY()
# Creates a shared library (.dll, .dylib, .so) using the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_SHARED_LIBRARY)

	ow_prepare_binary()

	add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRCS})

	ow_finish_binary()

endmacro (OW_CREATE_SHARED_LIBRARY)
