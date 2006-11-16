# - OW_COPY_DIR_RECURSIVE(src dst)
# Copies recursively a directory to ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${dst}
# only if src is different (newer) than dst
# See OWCopyDir(src dst)
# See OWCopyFile(src dst)
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_COPY_DIR_RECURSIVE src dst)

	get_filename_component(relativePath ${src} PATH)

	file(GLOB_RECURSE fileList ${src})
	if (fileList)
		foreach (file ${fileList})
			file(RELATIVE_PATH path ${relativePath} ${file})
			get_filename_component(path ${path} PATH)

			ow_copy_file(${file} ${dst}/${path}/.)
		endforeach (file ${fileList})
	endif (fileList)

endmacro (OW_COPY_DIR_RECURSIVE)
