# - OW_COPY_DIR(src dst)
# Copies a directory to ${dst} only if src is different (newer) than dst
# See OWCopyFile(src dst)
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_COPY_DIR src dst)

	file(GLOB fileList ${src})
	if (fileList)
		foreach (file ${fileList})
			ow_copy_file(${file} ${dst})
		endforeach (file ${fileList})
	endif (fileList)

endmacro (OW_COPY_DIR)
