ow_create_executable(owcrashreport)

ow_use_public_libraries(
	owutil
	owthread
	owsystem
	owcurl
	owqtutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(owcrashreport_SRCS
	FtpUpload.h
	QtCrashReport.h
)

ow_add_sources(
	main.cpp
	FtpUpload.cpp
	QtCrashReport.cpp
)

qt4_wrap_ui(owcrashreport_SRCS
	CrashReport.ui
)

ow_create_binary()
