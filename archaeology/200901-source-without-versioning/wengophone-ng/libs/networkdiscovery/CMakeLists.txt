ow_create_shared_library(networkdiscovery)

ow_use_public_libraries(
	netlib
	owutil
)

ow_use_private_libraries(
	owsl
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/EnumNatType.cpp
	src/EnumProxyAuthType.cpp
	src/NetworkDiscovery.cpp
	src/NetworkObserver.cpp
	src/NetworkProxyDiscovery.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/networkdiscovery/NetworkObserver.h
)

if (APPLE)
	ow_add_private_link_flags(
		# avoid a -multi_module error (see http://gcc.gnu.org/ml/gcc/2005-06/msg00184.html)
		-Wl,-single_module
		# avoid a "has local relocation entries in non-writable section" (see http://lists.mplayerhq.hu/pipermail/ffmpeg-devel/2006-October/017814.html)
		-read_only_relocs suppress
	)
endif (APPLE)

ow_create_project_binary()
