project(owmemorydump)

#owutil dependency
include_directories(${owutil_INCLUDE_DIR})
link_directories(${owutil_BINARY_DIR})

#includes
set(owmemorydump_INCLUDE_DIR
	${owmemorydump_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owmemorydump_INCLUDE_DIR})

#sources
set(owmemorydump_SRCS
	src/win32/MemoryDump.cpp
)

#owmemorydump
add_library(owmemorydump STATIC ${owmemorydump_SRCS})

#owutil dependency
target_link_libraries(owmemorydump owutil)
