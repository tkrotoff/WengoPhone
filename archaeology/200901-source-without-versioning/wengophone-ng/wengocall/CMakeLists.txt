ow_create_executable(wengocall)

ow_use_private_libraries(
	coipmanager
	coipmanager_threaded
	glib2
	qtgui
	owutil
)

if (APPLE)
	ow_use_private_frameworks(
		AudioToolbox
		AudioUnit
		Cocoa
		CoreAudio
		CoreFoundation
		QuickTime
	)
endif (APPLE)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	CallWidget.h
	LoginWidget.h
	QVideoWidget.h
)

qt4_wrap_ui(${PROJECT_NAME}_SRCS
	CallWidget.ui
	LoginWidget.ui
)

ow_add_sources(
	CallWidget.cpp
	LoginWidget.cpp
	QVideoWidget.cpp
	main.cpp
)

ow_create_project_binary(WIN32 MACOSX_BUNDLE)
