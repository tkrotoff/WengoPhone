project(owserializer)

#Boost dependency
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#owcutil dependency
include_directories(${owcutil_INCLUDE_DIR})
link_directories(${owcutil_BINARY_DIR})

#owutil dependency
include_directories(${owutil_INCLUDE_DIR})
link_directories(${owutil_BINARY_DIR})

#owserialization dependency
include_directories(${owserialization_INCLUDE_DIR})
link_directories(${owserialization_BINARY_DIR})

#tinyxml dependency
include_directories(${tinyxml_INCLUDE_DIR})
link_directories(${tinyxml_BINARY_DIR})

#includes
set(owserializer_INCLUDE_DIR
	${owserializer_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owserializer_INCLUDE_DIR})

#sources
set(owserializer_SRCS
	src/DateXMLSerializer.cpp
	src/TimeXMLSerializer.cpp
)

#owserializer
add_library(owserializer STATIC ${owserializer_SRCS})

#owcutil dependency
target_link_libraries(owserializer owcutil)

#owutil dependency
target_link_libraries(owserializer owutil)

#owserialization dependency
target_link_libraries(owserializer owserialization)
