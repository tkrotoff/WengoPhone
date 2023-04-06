ow_create_shared_library(owsettings)

ow_use_public_libraries(
	boost
	owserialization
	owserializer
	qtcore
)

ow_use_private_libraries(
	tinyxml
)

ow_add_sources(
	src/Settings.cpp
	src/AutomaticSettings.cpp
	src/SettingsXMLSerializer.cpp
	src/SettingsFileStorage.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	Settings.h
)

ow_create_project_binary()

subdirs(
	tests
)