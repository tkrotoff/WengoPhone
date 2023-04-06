ow_create_plugin_library(phapichatsessionmanager)

ow_use_private_libraries(
	coipmanager
	sipwrapper
	phapiwrapper
	owsettings
	phapi
)

ow_add_private_definitions(
	-DCOIPMANAGER_PLUGIN_DLL
	-DBUILD_COIPMANAGER_PLUGIN_DLL
)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	PhApiChatSession.cpp
	PhApiChatSessionManager.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	PhApiChatSessionManager.h
)

set(PROJECT_BINARY_COPY_DIR ${BUILD_DIR}/coip-plugins/chatsessionmanager/.)
ow_create_project_binary()
