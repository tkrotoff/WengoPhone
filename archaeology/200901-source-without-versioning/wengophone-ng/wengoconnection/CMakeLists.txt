ow_create_executable(wengoconnection)

ow_use_private_libraries(
	coipmanager_threaded
	qtgui
)

ow_use_public_libraries(
	#iaxclient
)

if (APPLE)
	ow_use_private_frameworks(
		QuickTime
	)
endif (APPLE)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	LoginWidget.h
	LogWidget.h
)

ow_add_sources(
	LoginWidget.cpp
	LogWidget.cpp
	main.cpp
)

qt4_wrap_ui(${PROJECT_NAME}_SRCS
	LoginWidget.ui
	LogWidget.ui
)

ow_create_project_binary(WIN32 MACOSX_BUNDLE)
