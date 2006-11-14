# - OW_COPY_FILE_IF_DIFFERENT(src dst)
# Copies src to dst only if src is different (newer) than dst
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_COPY_FILE_IF_DIFFERENT src dst)

	exec_program(${CMAKE_COMMAND}
		ARGS
			"-E copy_if_different ${src} ${dst}"
	)

endmacro (OW_COPY_FILE_IF_DIFFERENT)
