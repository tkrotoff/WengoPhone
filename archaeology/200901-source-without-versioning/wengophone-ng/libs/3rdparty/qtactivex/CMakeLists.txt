ow_create_project(qtactivex)

ow_use_public_libraries(
	qtcore
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	if (WIN32)
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
	endif (WIN32)
else (CMAKE_BUILD_TYPE MATCHES Debug)
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
