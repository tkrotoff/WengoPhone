ow_create_static_library(owbrowser)

ow_use_public_libraries(
	owutil
	qtgui
	qtactivex
)

ow_use_private_libraries(
	qtcoreutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/OWBrowser.cpp
	src/QtBrowser.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/owbrowser/OWBrowser.h
	include/owbrowser/QtBrowser.h
)

ow_create_project_binary()
