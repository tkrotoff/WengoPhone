ow_create_executable(httprequest-test)

ow_use_private_libraries(
	owhttp
	qtgui
)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
)

ow_add_sources(
	HttpRequestWindow.cpp
	main.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	HttpRequestWindow.h
)

qt4_wrap_ui(${PROJECT_NAME}_SRCS
	HttpRequestWindow.ui
)

ow_create_project_binary()
