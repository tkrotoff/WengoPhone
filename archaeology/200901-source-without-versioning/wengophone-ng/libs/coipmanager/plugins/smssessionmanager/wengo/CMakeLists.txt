ow_create_plugin_library(wengosmssessionmanager)

ow_use_private_libraries(
	coipmanager
)

ow_use_public_libraries(
	webservice
)

ow_add_private_definitions(
	-DCOIPMANAGER_PLUGIN_DLL
	-DBUILD_COIPMANAGER_PLUGIN_DLL
)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	WengoSMSSession.cpp
	WengoSMSSessionManager.cpp
)

set(PROJECT_BINARY_COPY_DIR ${BUILD_DIR}/coip-plugins/smssessionmanager/.)
ow_create_project_binary()
