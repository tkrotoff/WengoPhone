project(owserialization)

#Boost dependency
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#owcutil dependency
include_directories(${owcutil_INCLUDE_DIR})
link_directories(${owcutil_BINARY_DIR})

#owutil dependency
include_directories(${owutil_INCLUDE_DIR})
link_directories(${owutil_BINARY_DIR})

#includes
set(owserialization_INCLUDE_DIR
	${owserialization_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owserialization_INCLUDE_DIR})

#sources
set(owserialization_SRCS
	src/fake.cpp
)

#owserialization
add_library(owserialization STATIC ${owserialization_SRCS})

#owcutil dependency
target_link_libraries(owserialization owcutil)

#owutil dependency
target_link_libraries(owserialization owutil)
