ow_create_plugin_library(purpleconnectmanager)

ow_use_private_libraries(
	coipmanager
	purplewrapper
)

ow_add_private_definitions(
	-DCOIPMANAGER_PLUGIN_DLL
	-DBUILD_COIPMANAGER_PLUGIN_DLL
)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	PurpleConnect.cpp
	PurpleConnectedState.cpp
	PurpleConnectingState.cpp
	PurpleConnectManager.cpp
	PurpleDisconnectedState.cpp
	PurpleParamSetter.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	PurpleConnect.h
	PurpleDisconnectedState.h
)

set(PROJECT_BINARY_COPY_DIR ${BUILD_DIR}/coip-plugins/connectmanager/.)
ow_create_project_binary()
