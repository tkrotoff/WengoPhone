ow_create_executable(coipmanager-test)

ow_use_private_libraries(
	coipmanager
	glib2
)

ow_use_public_libraries(
	#iaxclient
)

if (APPLE)
	ow_use_private_frameworks(
		AudioToolBox
		AudioUnit
		Cocoa
		CoreAudio
		QuickTime
	)
endif (APPLE)

ow_add_sources(
	AccountCommand.cpp
	CallCommand.cpp
	ChatCommand.cpp
	CoIpManagerTest.cpp
	Command.cpp
	ConnectCommand.cpp
	ContactCommand.cpp
	ContactListCommand.cpp
	Menu.cpp
	main.cpp
	UserProfileCommand.cpp
)

ow_create_project_binary()
