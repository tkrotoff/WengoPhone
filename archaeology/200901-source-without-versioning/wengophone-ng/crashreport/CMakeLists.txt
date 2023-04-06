ow_create_executable(owcrashreport)

ow_use_public_libraries(
	ftpupload
	qtcoreutil
	qtevent
)

ow_use_private_libraries(
	owsystem
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Because of UIC and MOC
ow_add_private_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	QtCrashReport.h
)

qt4_wrap_ui(${PROJECT_NAME}_SRCS
	CrashReport.ui
)

ow_add_sources(
	main.cpp
	QtCrashReport.cpp
)

ow_create_project_binary()
