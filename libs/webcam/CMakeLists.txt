ow_create_project(owwebcam)

ow_use_public_libraries(
	owutil
	owpixertool
	owthread
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/WebcamDriver.cpp
	src/webcam.cpp
)

if (UNIX AND NOT WIN32 OR APPLE)
	ow_use_public_libraries(
		owthread
	)
endif (UNIX AND NOT WIN32 OR APPLE)

if (MSVC)
	ow_add_private_libraries(
		winmm
		strmiids
		comsupp
	)

	ow_add_sources(
		src/directx/DirectXWebcamDriver.cpp
		src/directx/DirectXWebcamUtils.cpp
	)
endif (MSVC)

if (UNIX AND NOT WIN32)
	if (APPLE)
		ow_use_public_frameworks(
			Carbon
			CoreFoundation
			CoreServices
			QuickTime
		)

		ow_add_sources(
			src/quicktime/QuicktimeWebcamDriver.cpp
		)
	else (APPLE)
		ow_add_sources(
			src/v4l/V4LWebcamDriver.cpp
		)
	endif (APPLE)
endif (UNIX AND NOT WIN32)

if (APPLE)
	ow_create_static_library()
else (APPLE)
	ow_add_public_definitions(
		-DOWWEBCAM_DLL
	)

	ow_add_private_definitions(
		-DOWWEBCAM_DLL
		-DBUILD_OWWEBCAM_DLL
	)

	ow_create_shared_library()
endif (APPLE)