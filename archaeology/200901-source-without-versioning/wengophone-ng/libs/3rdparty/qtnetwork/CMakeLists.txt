set(QT_MIN_VERSION 4.1.2)

if (NOT QT4_FOUND)
	find_package(Qt4 REQUIRED)
endif (NOT QT4_FOUND)

ow_create_project(qtnetwork)

ow_use_public_libraries(
	qtcore
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTNETWORK_LIBRARY_DEBUG}
	)
else (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${QT_QTNETWORK_LIBRARY_RELEASE}
	)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if (MSVC)
	string(REGEX REPLACE "\\\\" "/" QTDIR $ENV{QTDIR})

	if (CMAKE_BUILD_TYPE MATCHES Debug)
		ow_copy_file(${QTDIR}/bin/QtNetworkd4.dll ${BUILD_DIR})
	else (CMAKE_BUILD_TYPE MATCHES Debug)
		ow_copy_file(${QTDIR}/bin/QtNetwork4.dll ${BUILD_DIR})
	endif (CMAKE_BUILD_TYPE MATCHES Debug)
endif (MSVC)
