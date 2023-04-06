ow_create_shared_library(webservice)

ow_use_public_libraries(
	owutil
	owhttp
	coipmanager_base
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/WengoWebService.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/webservice/WengoWebService.h
)

ow_create_project_binary()
