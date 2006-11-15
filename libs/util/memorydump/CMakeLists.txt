ow_create_static_library(owmemorydump)

ow_use_public_libraries(
	owutil
)

if (MSVC)
	ow_add_private_libraries(
		shlwapi
	)
endif (MSVC)

ow_add_sources(
	src/win32/MemoryDump.cpp
)

ow_create_binary()
