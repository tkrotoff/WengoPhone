ow_create_executable(owcrashreport)

ow_use_public_libraries(
	owutil
	owthread
	owsystem
	owcurl
	qt4
	owqtutil
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
	FtpUpload.h
	QtCrashReport.h
)

qt4_wrap_ui(${PROJECT_NAME}_SRCS
	CrashReport.ui
)

ow_add_sources(
	main.cpp
	FtpUpload.cpp
	QtCrashReport.cpp
)

ow_create_binary()
