ow_create_executable(coipmanager-unit-tests)

ow_use_private_libraries(
	boost-test
	coipmanager
	glib2
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
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	main.cpp
)

ow_create_project_binary()
