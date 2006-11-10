ow_create_static_library(owmemorydump)

ow_use_public_libraries(
	owutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
	ow_add_private_libraries(
		shlwapi
	)
endif (MSVC)

#ow_add_public_definitions(
#	-DOWMEMORYDUMP_DLL
#)

#ow_add_private_definitions(
#	-DOWMEMORYDUMP_DLL
#	-DBUILD_OWMEMORYDUMP_DLL
#)

ow_add_sources(
	src/win32/MemoryDump.cpp
)

ow_create_binary()
