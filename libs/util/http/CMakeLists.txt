ow_create_project(owhttp)

ow_use_public_libraries(
	owthread
	owcurl
)

ow_use_private_libraries(
	owcurl
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (WIN32)
	ow_add_private_libraries(
		advapi32
		shell32
	)
endif (WIN32)

#ow_add_public_definitions(
#	-DOWHTTP_DLL
#)

#ow_add_private_definitions(
#	-DOWHTTP_DLL
#	-DBUILD_OWHTTP_DLL
#)

ow_add_sources(
	src/HttpRequest.cpp
	src/HttpRequestManager.cpp
	src/curl/CurlHttpRequest.cpp
	src/null/NullHttpRequest.cpp
	#src/qt/QtHttpRequest.cpp
)

if (APPLE)
	ow_use_private_frameworks(
		CoreFoundation
	)
endif (APPLE)

ow_create_static_library()
