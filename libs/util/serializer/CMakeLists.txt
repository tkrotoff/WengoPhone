ow_create_project(owserializer)

ow_use_public_libraries(
	owserialization
)

ow_use_private_libraries(
	tinyxml
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

#ow_add_public_definitions(
#	-DOWSERIALIZER_DLL
#)

#ow_add_private_definitions(
#	-DOWSERIALIZER_DLL
#	-DBUILD_OWSERIALIZER_DLL
#)

ow_add_sources(
	src/DateXMLSerializer.cpp
	src/TimeXMLSerializer.cpp
)

ow_create_static_library()
