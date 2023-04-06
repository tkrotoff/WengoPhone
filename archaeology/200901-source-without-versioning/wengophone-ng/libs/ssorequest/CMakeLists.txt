ow_create_shared_library(ssorequest)

ow_use_public_libraries(
	webservice
	tinyxml
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/SSORequest.cpp
	src/WengoAccountParser.cpp
)

ow_create_project_binary()
