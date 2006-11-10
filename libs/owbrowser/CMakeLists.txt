ow_create_static_library(owbrowser)

ow_use_public_libraries(
	owutil
	qt4
)

ow_use_private_libraries(
	owqtutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	QtBrowser.cpp
)

qt4_automoc(${owbrowser_SRCS})

ow_create_binary()
