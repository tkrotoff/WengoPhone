ow_create_shared_library(ftpupload)

ow_use_private_libraries(
	curl
	owutil
	qtcore
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/FtpUpload.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/ftpupload/FtpUpload.h
)

ow_create_project_binary()
