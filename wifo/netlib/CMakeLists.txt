ow_create_static_library(netlib)

ow_use_private_libraries(
	owcutil
	owcurl
	httptunnel
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
	ow_add_private_libraries(
		user32
		shell32
		ws2_32
		advapi32
		wininet
		urlmon
	)
endif (MSVC)

ow_add_sources(
	src/netlib.cpp
	src/netlib_util.cpp
	src/stun_func.cpp
	src/udp_func.cpp
	src/bugfix.cpp
)

ow_create_binary()
