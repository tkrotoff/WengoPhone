ow_create_plugin_library(yealinkwenbox)

# find precompiled ylusbtel
find_library(YLUSBTEL_LIBRARIES
	NAMES
		YLUSBTEL
	PATH
		${CMAKE_CURRENT_SOURCE_DIR}
)

# hack because the find_library does not work, why? no idea...
set(YLUSBTEL_LIBRARIES
	${CMAKE_CURRENT_SOURCE_DIR}/YLUSBTEL
)

ow_use_public_libraries(
	owwenbox
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_private_libraries(
	${YLUSBTEL_LIBRARIES}
)

ow_add_sources(
	YealinkWenbox.cpp
)

ow_create_binary()
