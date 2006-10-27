ow_create_project(owsettings)

ow_use_public_libraries(
	owserialization
	owthread
)

ow_use_private_libraries(
	tinyxml
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

#ow_add_public_definitions(
#	-DOWSETTINGS_DLL
#)

#ow_add_private_definitions(
#	-DOWSETTINGS_DLL
#	-DBUILD_OWSETTINGS_DLL
#)

ow_add_sources(
	src/Settings.cpp
	src/AutomaticSettings.cpp
	src/StringListXMLSerializer.cpp
	src/SettingsXMLSerializer.cpp
)

ow_create_static_library()
