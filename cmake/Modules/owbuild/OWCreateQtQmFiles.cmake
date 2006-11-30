# - OW_CREATE_QT_QM_FILES(qmFiles tsFile1 ... tsFileN)
# Creates .qm files from .ts files
#
# Copyright (C) 2006  Andreas Schneider <mail@cynapses.org>
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_CREATE_QT_QM_FILES qmFiles)

	# No double check for Qt4
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

		add_custom_command(
			OUTPUT
				${tsFile}
			COMMAND
				${QT_LUPDATE_EXECUTABLE}
				${${PROJECT_NAME}_SRCS}
				-ts ${tsFile}
				-noobsolete
				-verbose
			DEPENDS
				${${PROJECT_NAME}_SRCS}
			WORKING_DIRECTORY
				${CMAKE_CURRENT_SOURCE_DIR}
		)

		get_filename_component(basename ${tsFile} NAME_WE)

		get_filename_component(qmFile
			${BUILD_DIR}/${basename}.qm
			ABSOLUTE
		)

		add_custom_command(
			OUTPUT
				${qmFile}
			COMMAND
				${QT_LRELEASE_EXECUTABLE}
				${tsFile}
				-qm ${qmFile}
				-nounfinished
				-verbose
			DEPENDS
				${tsFile}
			WORKING_DIRECTORY
				${CMAKE_CURRENT_SOURCE_DIR}
		)

		set(${qmFiles} ${${qmFiles}} ${qmFile})

	endforeach(tsFile ${ARGN})

endmacro (OW_CREATE_QT_QM_FILES qmFiles)
