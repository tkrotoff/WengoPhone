ow_create_project(owphapiwrapper)

ow_use_public_libraries(
	owthread
	owsound
	owimwrapper
	owsipwrapper
	phapi
	sfp-plugin
)

ow_use_private_libraries(
	tinyxml
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_private_libraries(
	${ortp_LIBRARIES}
	${stun_LIBRARIES}
	${httptunnel_LIBRARIES}
)

ow_add_sources(
	PhApiFactory.cpp
	PhApiWrapper.cpp
	PhApiCallbacks.cpp
	PhApiIMChat.cpp
	PhApiIMConnect.cpp
	PhApiIMPresence.cpp
	PhApiCodecList.cpp
	PhApiSFPCallbacks.cpp
	PhApiSFPWrapper.cpp
	PhApiSFPEvent.cpp
)

if (WITH_VIDEO)
	ow_use_public_libraries(
		owpixertool
	)
	ow_add_private_definitions(
		-DENABLE_VIDEO
	)
endif (WITH_VIDEO)

ow_add_private_definitions(
	-DPHAPI_DLL
)

ow_create_static_library()

if (APPLE)
	ow_add_private_link_flags(
		"-framework CoreFoundation"
	)
endif (APPLE)
