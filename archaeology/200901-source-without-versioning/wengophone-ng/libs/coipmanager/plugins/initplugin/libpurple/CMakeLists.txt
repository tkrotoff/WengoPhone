ow_create_plugin_library(purpleinitplugin)

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
	PurpleInitPlugin.cpp
)

set(PROJECT_BINARY_COPY_DIR ${BUILD_DIR}/coip-plugins/initplugin/.)
ow_create_project_binary()
