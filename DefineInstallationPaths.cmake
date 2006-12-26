string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_DIR)
set(BUILD_DIR
	${CMAKE_BINARY_DIR}/${BUILD_DIR}
	CACHE PATH "Build directory, depends on build type" FORCE
)

if (WIN32)
	set(DATA_INSTALL_DIR ${BUILD_DIR}
		CACHE PATH "Data/resources (sounds, translations...) installation path")
endif (WIN32)

if (APPLE)
	set(DATA_INSTALL_DIR ${BUILD_DIR}/${APPLICATION_NAME}.app/Contents/Resources
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
