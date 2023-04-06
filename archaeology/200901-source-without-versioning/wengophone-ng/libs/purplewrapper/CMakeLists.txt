ow_create_shared_library(purplewrapper)

ow_use_public_libraries(
	libpurple
	coipmanager_base
)

ow_use_private_libraries(
	owcutil
	owutil
	glib2
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/PurpleEnumIMProtocol.cpp
	src/PurpleEnumPresenceState.cpp
	src/PurpleWrapper.cpp
)

if (UNIX)
	if (NOT APPLE)
		ow_add_private_definitions(
			-DENABLE_NLS=1
		)
	endif (NOT APPLE)
endif (UNIX)

ow_create_project_binary()
