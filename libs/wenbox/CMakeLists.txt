ow_create_static_library(owwenbox)

if (WIN32)
	subdirs(src/yealink)
endif (WIN32)

ow_use_public_libraries(
	owutil
)

ow_use_private_libraries(
	owshlibloader
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/Wenbox.cpp
)

ow_create_binary()
