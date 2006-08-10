project(owcutil)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(owcutil_SRCS src/fake.c
	src/strlcat.c
	src/strlcpy.c
)

if (MSVC)
	set(owcutil_SRCS ${owcutil_SRCS}
		src/msvc/dirent.c
	)
	include_directories(${CMAKE_SOURCE_DIR}/include/cutil/msvc)
endif(msvc)

add_library(owcutil STATIC ${owcutil_SRCS})
