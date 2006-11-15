ow_create_static_library(owcutil)

ow_add_sources(
	src/fake.c
	src/strlcat.c
	src/strlcpy.c
)

ow_add_public_include_dirs(
	${LIBUTIL_INCLUDE_DIRS}
)

if (MSVC)
	ow_add_public_include_dirs(
		${CMAKE_CURRENT_SOURCE_DIR}/msvc
	)
endif (MSVC)

if (MSVC)
	ow_add_sources(
		src/msvc/dirent.c
	)
endif (MSVC)

ow_create_binary()
