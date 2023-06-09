# This has to be reworked, what is the difference between:
# Data/resources (sounds, translations...) copy path
# and
# Data/resources (sounds, translations...) installation path
# ??
# TODO merge with branch 2.1
#
# set(CACHE PATH FORCE) FORCE is not a solution!!!

if (WIN32)
	set(DATA_COPY_DIR ${BUILD_DIR}
		CACHE PATH "Data/resources (sounds, translations...) copy path" FORCE)
endif (WIN32)

if (APPLE)
	set(DATA_COPY_DIR ${BUILD_DIR}/${APPLICATION_NAME}.app/Contents/Resources
		CACHE PATH "Data/resources (sounds, translations...) copy path" FORCE)
endif (APPLE)

if (LINUX)
	set(DATA_COPY_DIR ${BUILD_DIR}
		CACHE PATH "Data/resources (sounds, translations...) copy path")

	set(DATA_INSTALL_DIR "/usr/share/wengophone"
		CACHE PATH "Data/resources (sounds, translations...) installation path")
	set(APPLICATION_BIN_INSTALL_DIR "/usr/bin"
		CACHE PATH "Application binary installation path")
	set(LIBRARIES_INSTALL_DIR "/usr/lib/wengophone"
		CACHE PATH "Libraries installation path")
endif (LINUX)
