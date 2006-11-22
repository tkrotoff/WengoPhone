# - OW_CREATE_QT_QM_FILES(qmFiles tsFile1 ... tsFileN)
# Creates .qm files from .ts files
#
# Copyright (C) 2006  Andreas Schneider <mail@cynapses.org>
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_QT_QM_FILES qmFiles)

	if (NOT QT4_FOUND)
		find_package(Qt4 REQUIRED)
	endif (NOT QT4_FOUND)

	get_filename_component(qmakePath ${QT_QMAKE_EXECUTABLE} PATH)

	find_program(QT_LUPDATE_EXECUTABLE
		NAMES
			lupdate
		PATHS
			${qmakePath}
			$ENV{QTDIR}/bin
			"[HKEY_CURRENT_USER\\Software\\Trolltech\\Qt3Versions\\4.0.0;InstallDir]/bin"
			"[HKEY_CURRENT_USER\\Software\\Trolltech\\Versions\\4.0.0;InstallDir]/bin"
	)

	find_program(QT_LRELEASE_EXECUTABLE
		NAMES
			lrelease
		PATHS
			${qmakePath}
			$ENV{QTDIR}/bin
			"[HKEY_CURRENT_USER\\Software\\Trolltech\\Qt3Versions\\4.0.0;InstallDir]/bin"
			"[HKEY_CURRENT_USER\\Software\\Trolltech\\Versions\\4.0.0;InstallDir]/bin"
	)

	foreach(tsFile ${ARGN})

		message(STATUS "lupdate " ${tsFile})

		#add_custom_command(
		#	OUTPUT
		#		${tsFile}
		#	COMMAND
		#		${QT_LUPDATE_EXECUTABLE}
		#		${${PROJECT_NAME}_SRCS}
		#		-ts ${tsFile}
		#)

		execute_process(
			COMMAND
				${QT_LUPDATE_EXECUTABLE}
				${${PROJECT_NAME}_SRCS}
				-ts ${tsFile}
			WORKING_DIRECTORY
				${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE
				${tsFile}
		)

		get_filename_component(basename ${tsFile} NAME_WE)

		get_filename_component(qmFile
			${CMAKE_CURRENT_BINARY_DIR}/${basename}.qm
			ABSOLUTE
		)

		message(STATUS "lrelease " ${qmFile})

		#add_custom_command(
		#	OUTPUT
		#		${qmFile}
		#	COMMAND
		#		${QT_LRELEASE_EXECUTABLE}
		#		${tsFile}
		#		-qm ${qmFile}
		#)

		execute_process(
			COMMAND
				${QT_LRELEASE_EXECUTABLE}
				${tsFile}
				-qm ${qmFile}
			WORKING_DIRECTORY
				${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE
				${qmFile}
		)

		set(${qmFiles} ${${qmFiles}} ${qmFile})

	endforeach(tsFile ${ARGN})

endmacro (OW_CREATE_QT_QM_FILES qmFiles)
