# - OW_CHANGE_INSTALL_NAME(src dst file)
# MAC OS X ONLY!
# Calls "install_name_tool -change $src $dst $file"
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.

if (APPLE)
	macro (OW_CHANGE_INSTALL_NAME src dst file)

		exec_program("install_name_tool"
			ARGS
				"-change ${src} ${dst} ${file}"
		)

	endmacro (OW_CHANGE_INSTALL_NAME)
endif (APPLE)
