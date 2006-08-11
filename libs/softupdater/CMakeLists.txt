project(owsoftupdater)

#owhttp dependency
include_directories(${owhttp_INCLUDE_DIR})
link_directories(${owhttp_BINARY_DIR})

#includes
set(owsoftupdater_INCLUDE_DIR
	${owsoftupdater_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owsoftupdater_INCLUDE_DIR})

#sources
set(owsoftupdater_SRCS
	src/SoftUpdater.cpp
)

#owsoftupdater
add_library(owsoftupdater STATIC ${owsoftupdater_SRCS})

#owhttp dependency
target_link_libraries(owsoftupdater owhttp)
