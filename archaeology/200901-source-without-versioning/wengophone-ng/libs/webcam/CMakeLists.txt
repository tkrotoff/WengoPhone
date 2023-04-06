include(CMakeLists-owbuild.txt)

if (OWWEBCAM_MODULE)
	set(OWPIXERTOOL_MODULE TRUE)
	ow_svn_checkout(https://dev.openwengo.com/svn/openwengo/pixertool/trunk libs/pixertool)
	add_subdirectory(libs/pixertool)
endif (OWWEBCAM_MODULE)

ow_create_shared_library(webcam)

ow_use_public_libraries(
	owutil
	pixertool
	qtcore
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/WebcamDriver.cpp
	src/webcam.cpp
)


qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/webcam/IWebcamDriver.h
	src/WebcamFrameCaptured.h
)

if (MSVC)
	ow_add_private_libraries(
		winmm
		strmiids
		comsuppw
	)

	if (MSVC80)
		# Solve link error atlthunk.lib when using Visual C++ 2005 Express
		ow_add_private_link_flags(
			/nodefaultlib:atlthunk.lib
		)
	endif (MSVC80)

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

		qt4_wrap_cpp(${PROJECT_NAME}_SRCS
			include/webcam/V4LWebcamDriver.h
		)
		ow_add_sources(
			src/v4l/V4LWebcamDriver.cpp
		)
	endif (APPLE)
endif (UNIX AND NOT WIN32)

ow_create_project_binary()
