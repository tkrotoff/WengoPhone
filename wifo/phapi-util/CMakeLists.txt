ow_create_project(phapi-util)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
	ow_add_public_include_dirs(
		${CMAKE_CURRENT_SOURCE_DIR}/include/phapi-util/msvc
	)
endif (MSVC)

ow_add_sources(
	src/util.c
	src/comparator.c
	src/linkedlist.c
	src/mappinglist.c
	src/phSharedLib.c
)

if (WIN32)
	ow_add_private_definitions(
		-DWIN32
	)

	ow_add_sources(
		src/msvc/pthread.c
		src/msvc/unistd.c
	)
endif (WIN32)

ow_create_static_library()
