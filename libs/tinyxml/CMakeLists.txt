project(tinyxml)

#includes
set(tinyxml_INCLUDE_DIR
	${tinyxml_SOURCE_DIR}/
	CACHE INTERNAL ""
)

include_directories(${tinyxml_INCLUDE_DIR})

#sources
set(tinyxml_SRCS
	tinyxml.cpp
	tinyxmlerror.cpp
	tinyxmlparser.cpp
)

#tinyxml
add_library(tinyxml STATIC ${tinyxml_SRCS})
