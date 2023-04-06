set(QT_MIN_VERSION 4.2.0)

if (NOT QT4_FOUND)
	find_package(Qt4 REQUIRED)
endif (NOT QT4_FOUND)

ow_create_project(qtcore)

ow_add_public_include_dirs(
	${QT_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
)

# See http://www.cmake.org/pipermail/cmake/2005-August/007149.html
string(REGEX REPLACE "\\\\" "/" QT_LIBRARY_DIR ${QT_LIBRARY_DIR})

ow_add_public_library_dirs(
	${QT_LIBRARY_DIR}
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTCORE_LIBRARY_DEBUG}
	)
else (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTCORE_LIBRARY_RELEASE}
	)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

ow_add_public_definitions(
	-DQT_DLL
	-DQT_NO_KEYWORDS
	-DQT_CORE_LIB
)

if (MSVC)
	string(REGEX REPLACE "\\\\" "/" QTDIR $ENV{QTDIR})
	message(STATUS "** QTDIR (see qtcore/CMakeLists.txt)= ${QTDIR}")

	if (CMAKE_BUILD_TYPE MATCHES Debug)
		ow_copy_file(${QTDIR}/bin/QtCored4.dll ${BUILD_DIR})
	else (CMAKE_BUILD_TYPE MATCHES Debug)
		ow_copy_file(${QTDIR}/bin/QtCore4.dll ${BUILD_DIR})
	endif (CMAKE_BUILD_TYPE MATCHES Debug)
endif (MSVC)
