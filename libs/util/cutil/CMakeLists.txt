project(owcutil)

#includes
set(owcutil_INCLUDE_DIR
	${owcutil_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

if (MSVC)
	set(owcutil_INCLUDE_DIR ${owcutil_INCLUDE_DIR}
		${owcutil_SOURCE_DIR}/include/cutil/msvc
		CACHE INTERNAL ""
	)
endif (MSVC)

include_directories(${owcutil_INCLUDE_DIR})

#sources
set(owcutil_SRCS
	src/fake.c
	src/strlcat.c
	src/strlcpy.c
)

if (MSVC)
	set(owcutil_SRCS ${owcutil_SRCS}
		src/msvc/dirent.c
	)
endif (MSVC)

#owcutil
add_library(owcutil STATIC ${owcutil_SRCS})
