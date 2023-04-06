ow_create_static_library(owidle)

ow_use_public_libraries(
	owutil
	qtgui
)

ow_use_private_libraries(
	qtcoreutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/idle/Idle.h
)

ow_add_sources(
	src/Idle.cpp
)

ow_create_project_binary()
