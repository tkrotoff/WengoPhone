ow_create_shared_library(qtevent)

ow_use_public_libraries(
	owcutil
	owutil
	qtcore
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Because of UIC and MOC
ow_add_private_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/qtevent/QtThreadEvent.h
	include/qtevent/ThreadEventFilter.h
	include/qtevent/QtPostEvent.h
)

ow_add_sources(
	src/QtThreadEvent.cpp
	src/ThreadEventFilter.cpp
	src/QtPostEvent.cpp
)

ow_create_project_binary()
