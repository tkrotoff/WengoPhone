ow_create_static_library(iaxclientwrapper)

ow_use_public_libraries(
	sipwrapper
	iaxclient
	sound
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	IAXClientFactory.cpp
	IAXClientWrapper.cpp
)

ow_create_project_binary()
