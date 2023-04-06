ow_create_shared_library(owserialization)

ow_use_public_libraries(
	owutil
)

ow_add_sources(
	src/Serializable.cpp
	src/Storage.cpp
)

ow_create_project_binary()
