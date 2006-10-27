ow_create_project(owutil)

if (UNIX AND NOT WIN32 AND NOT APPLE)
	find_package(UUID REQUIRED)
endif (UNIX AND NOT WIN32 AND NOT APPLE)

ow_use_public_libraries(
	owcutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${BOOST_INCLUDE_DIRS}
)

ow_add_private_libraries(
	${BOOST_SIGNALS_LIBRARY}
)

#ow_add_public_definitions(
#	-DOWUTIL_DLL
#)

#ow_add_private_definitions(
#	-DOWUTIL_DLL
#	-DBUILD_OWUTIL_DLL
#)

ow_add_sources(
	src/exception/Exception.cpp
	src/Base64.cpp
	src/CountryList.cpp
	src/Date.cpp
	src/File.cpp
	src/Logger.cpp
	src/OWPicture.cpp
	src/Path.cpp
	src/String.cpp
	src/StringList.cpp
	src/Time.cpp
	src/Uuid.cpp
	src/WebBrowser.cpp
)

if (WIN32)
	ow_add_sources(
		src/win/UuidWin.cpp
	)
	ow_add_private_libraries(
		Rpcrt4
	)
endif (WIN32)

if (UNIX AND NOT WIN32)
	if (APPLE)
		ow_add_sources(
			src/mac/UuidMac.cpp
		)
	else (APPLE)
		ow_use_public_libraries(
			UUID
		)
		ow_add_sources(
			src/unix/UuidUnix.cpp
		)
	endif (APPLE)
endif (UNIX AND NOT WIN32)

if (APPLE)
	ow_add_private_link_flags(
		-framework CoreFoundation
	)
endif (APPLE)

ow_create_static_library()
