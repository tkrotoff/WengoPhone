if (NOT BOOST_FOUND)
	find_package(Boost REQUIRED)
endif (NOT BOOST_FOUND)

ow_create_project(boost)

ow_use_public_libraries(
	BOOST
)

if (NOT MSVC)
	set(boost_LIBRARIES
		${BOOST_PROGRAM_OPTIONS_LIBRARY}
		${BOOST_REGEX_LIBRARY}
		${BOOST_SERIALIZATION_LIBRARY}
		${BOOST_SIGNALS_LIBRARY}
		${BOOST_THREAD_LIBRARY}
		CACHE INTERNAL "${PROJECT_NAME} libraries"
	)
endif (NOT MSVC)

ow_add_public_definitions(
	-DBOOST_ALL_DYN_LINK
	${BOOST_LIB_DIAGNOSTIC_DEFINITIONS}
)

if (MSVC)
	# Disable warnings with templates under Visual C++
	ow_add_public_definitions(
		/wd4251
		/wd4275
		/wd4675
	)

	ow_copy_file(${BOOST_LIBRARY_DIRS}/boost_signals${BOOST_LIB_SUFFIXES}-1_33_1.dll ${BUILD_DIR})
	ow_copy_file(${BOOST_LIBRARY_DIRS}/boost_serialization${BOOST_LIB_SUFFIXES}-1_33_1.dll ${BUILD_DIR})
	ow_copy_file(${BOOST_LIBRARY_DIRS}/boost_program_options${BOOST_LIB_SUFFIXES}-1_33_1.dll ${BUILD_DIR})
	ow_copy_file(${BOOST_LIBRARY_DIRS}/boost_regex${BOOST_LIB_SUFFIXES}-1_33_1.dll ${BUILD_DIR})
endif (MSVC)
