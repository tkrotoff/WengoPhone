ow_create_shared_library(owhttp)

ow_use_public_libraries(
	curl
	owutil
	#qtnetwork
)

ow_use_private_libraries(
	curl
)

if (WIN32)
	ow_add_private_libraries(
		advapi32
		shell32
	)
endif (WIN32)

ow_add_sources(
	src/HttpRequest.cpp
	src/curl/CurlHttpRequest.cpp
	#src/null/NullHttpRequest.cpp
	#src/qt/QtHttpRequest.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	IHttpRequest.h
	HttpRequest.h
	src/curl/CurlHttpRequest.h
	#src/qt/QtHttpRequest.h
)

if (APPLE)
	ow_use_private_frameworks(
		CoreFoundation
	)
endif (APPLE)

ow_create_project_binary()

subdirs(
	tests/test2
)
