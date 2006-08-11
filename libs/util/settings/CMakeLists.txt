project(owsettings)

#owutil dependency
link_directories(${owutil_BINARY_DIR})

#owserialization dependency
include_directories(${owserialization_INCLUDE_DIR})
link_directories(${owserialization_BINARY_DIR})

#owthread dependency
include_directories(${owthread_INCLUDE_DIR})
link_directories(${owthread_BINARY_DIR})

#tinyxml dependency
include_directories(${tinyxml_INCLUDE_DIR})

#includes
set(owsettings_INCLUDE_DIR
	${owsettings_SOURCE_DIR}/include
	${owutil_INCLUDE_DIR}
	CACHE INTERNAL ""
)

include_directories(${owsettings_INCLUDE_DIR})

#sources
set(owsettings_SRCS
	src/Settings.cpp
	src/AutomaticSettings.cpp
	src/StringListXMLSerializer.cpp
	src/SettingsXMLSerializer.cpp
)

#owsettings
add_library(owsettings STATIC ${owsettings_SRCS})

#owutil dependency
target_link_libraries(owsettings owutil)
