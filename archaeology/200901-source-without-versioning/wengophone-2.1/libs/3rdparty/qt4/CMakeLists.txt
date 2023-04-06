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
		if (QT_QTUITOOLS_LIBRARY_DEBUG)
			ow_add_public_libraries(
				${QT_QTUITOOLS_LIBRARY_DEBUG}
			)
		else (QT_QTUITOOLS_LIBRARY_DEBUG)
			if (QT_QTUITOOLS_LIBRARY_RELEASE)
				ow_add_public_libraries(
					${QT_QTUITOOLS_LIBRARY_RELEASE}
				)
			else (QT_QTUITOOLS_LIBRARY_RELEASE)
				ow_add_public_libraries(
					QtUiTools
				)
			endif (QT_QTUITOOLS_LIBRARY_RELEASE)
		endif (QT_QTUITOOLS_LIBRARY_DEBUG)
	else (UNIX)
		ow_add_public_libraries(
			QtUiToolsd
		)

		find_library(
				QT_QTAXCONTAINER_LIBRARY_DEBUG
			NAMES
				QAxContainerd
			PATHS
				${QT_LIBRARY_DIR}
		)
		if (QT_QTAXCONTAINER_LIBRARY_DEBUG)
			message(STATUS "Qt4 with ActiveX (QAxContainer) support on")
			ow_add_public_libraries(
				${QT_QTAXCONTAINER_LIBRARY_DEBUG}
			)
		else (QT_QTAXCONTAINER_LIBRARY_DEBUG)
			message(STATUS "Qt4 with ActiveX (QAxContainer) support off")
		endif (QT_QTAXCONTAINER_LIBRARY_DEBUG)
	endif (UNIX)
else (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTCORE_LIBRARY_RELEASE}
		${QT_QTGUI_LIBRARY_RELEASE}
		${QT_QTXML_LIBRARY_RELEASE}
		${QT_QTSVG_LIBRARY_RELEASE}
		QtUiTools
	)

	if (WIN32)
		find_library(
				QT_QTAXCONTAINER_LIBRARY_RELEASE
			NAMES
				QAxContainer
			PATHS
				${QT_LIBRARY_DIR}
		)

		if (QT_QTAXCONTAINER_LIBRARY_RELEASE)
			message(STATUS "Qt4 with ActiveX (QAxContainer) support on")
			ow_add_public_libraries(
				${QT_QTAXCONTAINER_LIBRARY_RELEASE}
			)
		else (QT_QTAXCONTAINER_LIBRARY_RELEASE)
			message(STATUS "Qt4 with ActiveX (QAxContainer) support off")
		endif (QT_QTAXCONTAINER_LIBRARY_RELEASE)
	endif (WIN32)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

ow_add_public_definitions(
	-DQT_DLL
	-DQT_NO_KEYWORDS
)

if (MSVC)
	string(REGEX REPLACE "\\\\" "/" QTDIR $ENV{QTDIR})
	string(REGEX REPLACE "\\\\" "/" QT_PLUGINS_DIR ${QT_PLUGINS_DIR})

	if (CMAKE_BUILD_TYPE MATCHES Debug)
		ow_copy_file(${QTDIR}/bin/QtCored4.dll ${BUILD_DIR})
		ow_copy_file(${QTDIR}/bin/QtGuid4.dll ${BUILD_DIR})
		ow_copy_file(${QTDIR}/bin/QtXmld4.dll ${BUILD_DIR})
		ow_copy_file(${QTDIR}/bin/QtSvgd4.dll ${BUILD_DIR})
		#${BUILD_DIR}/imageformats/qmngd1.dll first then it creates imageformats directory
		ow_copy_file(${QT_PLUGINS_DIR}/imageformats/qmngd1.dll ${BUILD_DIR}/imageformats/qmngd1.dll)
		ow_copy_file(${QT_PLUGINS_DIR}/imageformats/qjpegd1.dll ${BUILD_DIR}/imageformats)
		ow_copy_file(${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/imageformats/qgifd1.dll ${BUILD_DIR}/imageformats)
	else (CMAKE_BUILD_TYPE MATCHES Debug)
		ow_copy_file(${QTDIR}/bin/QtCore4.dll ${BUILD_DIR})
		ow_copy_file(${QTDIR}/bin/QtGui4.dll ${BUILD_DIR})
		ow_copy_file(${QTDIR}/bin/QtXml4.dll ${BUILD_DIR})
		ow_copy_file(${QTDIR}/bin/QtSvg4.dll ${BUILD_DIR})
		#${BUILD_DIR}/imageformats/qmng1.dll first then it creates imageformats directory
		ow_copy_file(${QT_PLUGINS_DIR}/imageformats/qmng1.dll ${BUILD_DIR}/imageformats/qmng1.dll)
		ow_copy_file(${QT_PLUGINS_DIR}/imageformats/qjpeg1.dll ${BUILD_DIR}/imageformats)
		ow_copy_file(${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/imageformats/qgif1.dll ${BUILD_DIR}/imageformats)
	endif (CMAKE_BUILD_TYPE MATCHES Debug)
endif (MSVC)

if (LINUX)
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		set(suffix "_debug")
	else (CMAKE_BUILD_TYPE MATCHES Debug)
		set(suffix "")
	endif (CMAKE_BUILD_TYPE MATCHES Debug)

	file(GLOB plugin_list "${QT_PLUGINS_DIR}/imageformats/*${suffix}.so")
	foreach(plugin_path ${plugin_list})
		get_filename_component(plugin_name "${plugin_path}" NAME)
		ow_copy_file(${plugin_path} ${BUILD_DIR}/imageformats/${plugin_name})
	endforeach(plugin_path)
endif (LINUX)


# Copy Qt translations
exec_program(${QT_QMAKE_EXECUTABLE}
	ARGS "-query QT_INSTALL_TRANSLATIONS"
	OUTPUT_VARIABLE QT_INSTALL_TRANSLATIONS)

string(REGEX REPLACE "\\\\" "/" QT_INSTALL_TRANSLATIONS ${QT_INSTALL_TRANSLATIONS})

ow_copy_dir(${QT_INSTALL_TRANSLATIONS}/*.qm ${LOCALE_COPY_DIR}/.)
