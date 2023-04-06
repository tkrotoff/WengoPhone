ow_create_shared_library(phapiwrapper)

ow_use_public_libraries(
	owutil
	phapi
	sound
	sipwrapper
	sfp-plugin
)

ow_use_private_libraries(
	qtcore
	tinyxml
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_private_libraries(
	${ortp_LIBRARIES}
	${stun_LIBRARIES}
)

ow_add_sources(
	PhApiCallbacks.cpp
	PhApiChatWrapper.cpp
	PhApiCodecList.cpp
	PhApiContactPresenceWrapper.cpp
	PhApiFactory.cpp
	PhApiSFPCallbacks.cpp
	PhApiSFPWrapper.cpp
	PhApiWrapper.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	PhApiSFPWrapper.h
	PhApiWrapper.h
)

if (PHAPI_VIDEO_SUPPORT)
	ow_use_public_libraries(
		pixertool
	)
	ow_add_private_definitions(
		-DENABLE_VIDEO
	)
endif (PHAPI_VIDEO_SUPPORT)

ow_add_private_definitions(
	-DPHAPI_DLL
	-DPHAPIWRAPPER_DLL
)

if (APPLE)
	ow_use_private_frameworks(
		CoreFoundation
	)
endif (APPLE)

ow_create_project_binary()
