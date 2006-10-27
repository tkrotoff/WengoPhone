ow_create_project(owserialization)

ow_use_public_libraries(
	owutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

#ow_add_public_definitions(
#	-DOWSERIALIZATION_DLL
#)

#ow_add_private_definitions(
#	-DOWSERIALIZATION_DLL
#	-DBUILD_OWSERIALIZATION_DLL
#)

ow_add_sources(
	src/fake.cpp
)

ow_create_static_library()
