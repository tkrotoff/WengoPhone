set(QT_MIN_VERSION 4.1.2)

if (NOT QT4_FOUND)
	find_package(Qt4 REQUIRED)
endif (NOT QT4_FOUND)

ow_create_project(qt4)

ow_add_public_include_dirs(
	${QT_INCLUDE_DIR}
)

# see http://www.cmake.org/pipermail/cmake/2005-August/007149.html
string(REGEX REPLACE "\\\\" "/" QT_LIBRARY_DIR ${QT_LIBRARY_DIR})

ow_add_public_library_dirs(
	${QT_LIBRARY_DIR}
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTCORE_LIBRARY_DEBUG}
		${QT_QTGUI_LIBRARY_DEBUG}
		${QT_QTXML_LIBRARY_DEBUG}
		${QT_QTSVG_LIBRARY_DEBUG}
	)

	if (UNIX)
		ow_add_public_libraries(
			QtUiTools_debug
		)
	else (UNIX)
		ow_add_public_libraries(
			QtUiToolsd
			QAxContainerd
		)
	endif (UNIX)
else (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTCORE_LIBRARY_RELEASE}
		${QT_QTGUI_LIBRARY_RELEASE}
		${QT_QTXML_LIBRARY_RELEASE}
		${QT_QTSVG_LIBRARY_RELEASE}
		QtUiTools
		QAxContainer
	)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

ow_add_public_definitions(
	-DQT_DLL
	-DQT_NO_KEYWORDS
)

if (MSVC)
	string(REGEX REPLACE "\\\\" "/" QTDIR $ENV{QTDIR})

	ow_copy_dir(${QTDIR}/bin/*.dll .)
endif (MSVC)
