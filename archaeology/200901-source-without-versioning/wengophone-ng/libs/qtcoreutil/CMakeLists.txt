ow_create_shared_library(qtcoreutil)

ow_use_public_libraries(
	qtcore
	qtgui
	owutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Because of UIC and MOC
#ow_add_private_include_dirs(
#	${CMAKE_CURRENT_BINARY_DIR}
#	${CMAKE_CURRENT_SOURCE_DIR}
#)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/qtcoreutil/EventFilter.h
)

ow_add_sources(
	src/EventFilter.cpp
	src/KeyEventFilter.cpp
	src/MouseEventFilter.cpp
	src/PaintEventFilter.cpp
	src/EnabledChangeEventFilter.cpp
	src/ResizeEventFilter.cpp
	src/LanguageChangeEventFilter.cpp
	src/CloseEventFilter.cpp
	src/StringListConvert.cpp
)

ow_create_project_binary()
