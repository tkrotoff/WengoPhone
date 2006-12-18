if (APPLE)
	set(FRAMEWORK_INSTALL_DIR ${APPLICATION_NAME}.app/Contents/Frameworks
		CACHE PATH "Framework installation path (MacOSX only)")

	set(DATA_INSTALL_DIR ${APPLICATION_NAME}.app/Contents/Resources
		CACHE PATH "Data/resources (sounds, translations...) installation path")
endif (APPLE)

if (UNIX AND NOT APPLE)
	set(DATA_INSTALL_DIR "/usr/share/wengophone"
		CACHE PATH "Data/resources (sounds, translations...) installation path")
	set(APPLICATION_BIN_INSTALL_DIR "/usr/bin"
		CACHE PATH "Application binary installation path")
	set(LIBRARIES_INSTALL_DIR "/usr/lib/wengophone"
		CACHE PATH "Libraries installation path")
endif (UNIX AND NOT APPLE)
