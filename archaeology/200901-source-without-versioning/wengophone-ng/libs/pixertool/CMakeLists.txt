include(CMakeLists-owbuild.txt)

if (OWPIXERTOOL_MODULE)
	ow_svn_checkout(https://dev.openwengo.com/svn/openwengo/owbuild/trunk/libs-3rdparty-cmakelists/ffmpeg libs/3rdparty/ffmpeg)
	add_subdirectory(libs/3rdparty/ffmpeg)
	set(OWUTIL_MODULE TRUE)
	ow_svn_checkout(https://dev.openwengo.com/svn/openwengo/owutil/trunk libs/owutil)
	add_subdirectory(libs/owutil)
endif (OWPIXERTOOL_MODULE)

ow_create_shared_library(pixertool)

ow_use_public_libraries(
	owcutil
	ffmpeg
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/pixertool.cpp
	src/ffmpeg/ffmpeg-pixertool.c
)

if (MSVC)
	ow_add_private_libraries(
		winmm
		strmiids
		comsupp
	)

	ow_add_sources(
		src/directx/directx-pixertool.cpp
	)
endif (MSVC)

if (UNIX AND NOT WIN32)
	if (APPLE)
		ow_add_sources(
			src/quicktime/quicktime-pixertool.c
		)

		ow_add_private_link_flags(
			# avoid a -multi_module error (see http://gcc.gnu.org/ml/gcc/2005-06/msg00184.html)
			-Wl,-single_module
			# avoid a "has local relocation entries in non-writable section" (see http://lists.mplayerhq.hu/pipermail/ffmpeg-devel/2006-October/017814.html)
			-read_only_relocs suppress
		)
	else (APPLE)
		ow_add_sources(
			src/v4l/v4l-pixertool.c
		)
	endif (APPLE)
endif (UNIX AND NOT WIN32)

ow_create_project_binary()

#subdirs(
#	tests
#)
