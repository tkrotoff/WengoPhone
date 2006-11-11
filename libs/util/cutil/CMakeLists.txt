ow_create_static_library(owcutil)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/fake.c
	src/strlcat.c
	src/strlcpy.c
)

if (MSVC)
	ow_add_public_include_dirs(
		${CMAKE_CURRENT_SOURCE_DIR}/include/cutil/msvc
	)
endif (MSVC)

if (MSVC)
	ow_add_sources(
		src/msvc/dirent.c
	)
endif (MSVC)

ow_create_binary()
