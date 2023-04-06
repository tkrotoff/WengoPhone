# TODO : test dir

subdirs(
	pics
	sounds
	src/buildid
	src
	#test
)

if (COPY_DATA_FILES_ON_CONFIGURE)
	ow_copy_file(${CMAKE_CURRENT_SOURCE_DIR}/COPYING ${BUILD_DIR})
endif (COPY_DATA_FILES_ON_CONFIGURE)
