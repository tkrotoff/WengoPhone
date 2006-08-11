project(owcutil)

include_directories(include)

set(owcutil_SRCS
	src/fake.c
	src/strlcat.c
	src/strlcpy.c
)

if (MSVC)
	set(owcutil_SRCS ${owcutil_SRCS}
		src/msvc/dirent.c
	)
	include_directories(include/cutil/msvc)
endif (MSVC)

add_library(owcutil STATIC ${owcutil_SRCS})
