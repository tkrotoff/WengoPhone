#ow_create_shared_library(coipxpcom)
ow_create_static_library(coipxpcom)

ow_use_private_libraries(
	coipmanager_threaded
	owsystem
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
)

ow_add_sources(
	Command.cpp
	SystemTools.cpp
)

ow_create_project_binary(EXCLUDE_FROM_ALL)
