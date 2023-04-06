ow_create_executable(wengochat)

ow_use_private_libraries(
	coipmanager
	coipmanager_threaded
	glib2
	qtgui
	owutil
	qtcoreutil
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

ow_add_public_include_dirs(
	${CMAKE_CURRENT_BINARY_DIR}
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	ChatWidget.h
	EditAccount.h
	IMContactDialog.h
	LoggerWidget.h
	ManageAccount.h
	WengoChat.h
)

ow_add_sources(
	ChatWidget.cpp
	EditAccount.cpp
	IMContactDialog.cpp
	LoggerWidget.cpp
	ManageAccount.cpp
	WengoChat.cpp
	main.cpp
	Widget.cpp
	Config.cpp
)

qt4_wrap_ui(${PROJECT_NAME}_SRCS
	ChatWidget.ui
	IMContactDialog.ui
	LoginPasswordDialog.ui
	LoggerWidget.ui
	MainWindow.ui
	ManageAccount.ui
)

ow_create_project_binary(WIN32 MACOSX_BUNDLE)
