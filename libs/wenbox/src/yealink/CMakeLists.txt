project(owwenboxplugin)

#Boost dependency
find_package(Boost REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})

#owwenbox dependency
link_directories(${owwenbox_BINARY_DIR})
include_directories(${owwenbox_INCLUDE_DIR})

#ylusbtel.lib dependency
link_directories(${owwenboxplugin_SOURCE_DIR})

#includes
include_directories(.)

#sources
set(owwenboxplugin_SRCS
	YealinkWenbox.cpp
)

#owwenboxplugin
add_library(owwenboxplugin SHARED ${owwenboxplugin_SRCS})

#owwenbox dependency
target_link_libraries(owwenboxplugin owwenbox)

#ylusbtel
target_link_libraries(owwenboxplugin ylusbtel)
