ow_create_shared_library(owsocket)

ow_use_public_libraries(
	owutil
	qtcore
)

ow_add_sources(
	src/ClientSocket.cpp
	src/ServerSocket.cpp
)

ow_add_public_include_dirs(
	${LIBUTIL_INCLUDE_DIRS}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	ClientSocket.h
	ServerSocket.h
)

if (WIN32)
	ow_add_private_libraries(
		Ws2_32
	)
endif (WIN32)

ow_create_project_binary()
