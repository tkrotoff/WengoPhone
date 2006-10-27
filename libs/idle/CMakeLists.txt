ow_create_project(owidle)

include(${QT_USE_FILE})

ow_use_public_libraries(
	owutil
)

ow_use_private_libraries(
	owqtutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_private_libraries(
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
)

qt4_wrap_cpp(owidle_SRCS
	include/idle/Idle.h
)

ow_add_sources(
	src/Idle.cpp
)

ow_create_static_library()
