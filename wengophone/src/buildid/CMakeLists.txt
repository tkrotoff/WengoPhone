ow_create_static_library(owbuildid)

ow_use_private_libraries(
	glib2
	gaim
	tinyxml
	owcurl
	ffmpeg
	owutil
)

if (NOT APPLE)
	ow_use_private_libraries(
		PORTAUDIO
	)
endif (NOT APPLE)

if (WITH_BUILDID)
	ow_add_private_definitions(
		-DDD_BUILDID=${SVN_REVISION}LL
		-DDD_VERSION=${APPLICATION_VERSION}
		-DDD_REVISION=${SVN_REVISION}
		-DDD_SOFTPHONE_NAME=wengo
	)
else (WITH_BUILDID)
	ow_add_private_definitions(
		-DDD_BUILDID=0LL
		-DDD_VERSION=0
		-DDD_REVISION=0
		-DDD_SOFTPHONE_NAME=wengo
	)
endif (WITH_BUILDID)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	WengoPhoneBuildId.cpp
)

ow_create_binary()
