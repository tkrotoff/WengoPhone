include(../CMakeLists-common.txt)

message(STATUS "Generating package...")

if (APPLE)
	include(CMakeLists-package-macosx.txt)
else (APPLE)
	# Create a zip of CoIpManager SDK
	ow_zip_directory(${BUNDLE_DIR} ${CMAKE_BINARY_DIR}/${COIPMANAGER_BUNDLE_NAME}-revnumber)
endif (APPLE)
