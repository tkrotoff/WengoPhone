if (APPLE)
	ow_create_static_library(sfp-plugin)
else (APPLE)
	ow_create_shared_library(sfp-plugin)
endif (APPLE)

ow_use_public_libraries(
	phapi-util
)

ow_use_private_libraries(
	phapi
	exosip
	owcurl
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
	ow_add_private_definitions(
		-DBUILD_SFP_PLUGIN_DLL
		-DWIN32
	)
endif (MSVC)

ow_add_sources(
	src/sfp-parser.c
	src/sfp-plugin.c
	src/sfp-transfer.c
)

ow_create_binary()
