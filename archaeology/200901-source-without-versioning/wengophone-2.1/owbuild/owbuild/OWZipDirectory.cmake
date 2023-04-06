# - ow_zip_directory(src dst)
# Creates a zip archive of a directory
#
# On Windows, it uses the '7z' executable, provided by 7-Zip
# On Linux and Mac, it uses the 'zip' command
#
# Example:
# ow_zip_directory(src dst)
#
# Output generated:
# On Windows: 7z a -tZip -axdst dst src
#	-ax flag is used to exclude the archive to be archived.
#		This could happen if your try to zip the directory
#		where the archive has already been generated
#
#
# On Unix: zip dst src -x dst
#
#	-x exclude the archive. See above for reason.
#
# Copyright (C) 2006-2007  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (ow_zip_directory src dst)

	message(STATUS "Creating file: ${dst}.(zip|tar.bz2)")

	file(RELATIVE_PATH relativePathSrc ${CMAKE_CURRENT_BINARY_DIR} ${src})
	file(RELATIVE_PATH relativePathDst ${CMAKE_CURRENT_BINARY_DIR} ${dst})

	if (WIN32)
		find_program(7ZIP_EXECUTABLE 7z
			PATHS
				"C:/Program Files/7-Zip"
		)

		# 7z.exe a -tZip -mx9 -ax dst.zip dst.zip src
		# -mx9 means maximum compression
		execute_process(COMMAND "${7ZIP_EXECUTABLE}" a -tZip -mx9 "-ax${relativePathDst}.zip" "${relativePathDst}.zip" "${relativePathSrc}")
	endif (WIN32)

	if (DISABLED)
		find_program(ZIP_EXECUTABLE zip)

		# zip -9 -r dst.zip src -x dst.zip
		# -9 means maximum compression
		execute_process(COMMAND "${ZIP_EXECUTABLE}" "-9" "-r" "${relativePathDst}.zip" "${relativePathSrc}" "-x" "${relativePathDst}.zip")
	endif (DISABLED)

	if (UNIX)
		find_program(TAR_EXECUTABLE tar)

		# tar -cvjf dst.tar.bz2 src
		message("${TAR_EXECUTABLE}" "-cvjf" "${relativePathDst}.tar.bz2" "${relativePathSrc}")
		execute_process(
			COMMAND "${TAR_EXECUTABLE}" -cvjf "${relativePathDst}.tar.bz2" "${relativePathSrc}"
		)
	endif (UNIX)

endmacro (ow_zip_directory)
