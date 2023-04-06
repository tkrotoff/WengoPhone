set(QT_MIN_VERSION 4.1.2)

if (NOT QT4_FOUND)
	find_package(Qt4 REQUIRED)
endif (NOT QT4_FOUND)

ow_create_project(qtgui)

ow_use_public_libraries(
	qtcore
)

ow_add_public_include_dirs(
	${QT_QTGUI_INCLUDE_DIR}
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTGUI_LIBRARY_DEBUG}
	)
else (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTGUI_LIBRARY_RELEASE}
		${QT_QTMAIN_LIBRARY_RELEASE} # For WinMain under Windows
	)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if (MSVC)
	string(REGEX REPLACE "\\\\" "/" QTDIR $ENV{QTDIR})
	string(REGEX REPLACE "\\\\" "/" QT_PLUGINS_DIR ${QT_PLUGINS_DIR})

	if (CMAKE_BUILD_TYPE MATCHES Debug)
		ow_copy_file(${QTDIR}/bin/QtGuid4.dll ${BUILD_DIR})
		#${BUILD_DIR}/imageformats/qmngd1.dll first then it creates imageformats directory
		ow_copy_file(${QT_PLUGINS_DIR}/imageformats/qmngd1.dll ${BUILD_DIR}/imageformats/qmngd1.dll)
		ow_copy_file(${QT_PLUGINS_DIR}/imageformats/qjpegd1.dll ${BUILD_DIR}/imageformats)
		ow_copy_file(${CMAKE_CURRENT_SOURCE_DIR}/binary-lib/msvc/imageformats/qgifd1.dll ${BUILD_DIR}/imageformats)
	else (CMAKE_BUILD_TYPE MATCHES Debug)
		ow_copy_file(${QTDIR}/bin/QtGui4.dll ${BUILD_DIR})
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

	file(GLOB pluginList "${QT_PLUGINS_DIR}/imageformats/*${suffix}.so")
	foreach(pluginPath ${pluginList})
		get_filename_component(pluginName "${pluginPath}" NAME)
		ow_copy_file(${pluginPath} ${BUILD_DIR}/imageformats/${pluginName})
	endforeach(pluginPath)
endif (LINUX)

# Copy Qt translations
exec_program(${QT_QMAKE_EXECUTABLE}
	ARGS "-query QT_INSTALL_TRANSLATIONS"
	OUTPUT_VARIABLE QT_INSTALL_TRANSLATIONS)

string(REGEX REPLACE "\\\\" "/" QT_INSTALL_TRANSLATIONS ${QT_INSTALL_TRANSLATIONS})

ow_copy_dir(${QT_INSTALL_TRANSLATIONS}/*.qm ${LOCALE_COPY_DIR}/.)
