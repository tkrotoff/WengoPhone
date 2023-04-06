ow_create_project(boost-test)

ow_use_public_libraries(
	boost
)

if (NOT BOOST_FOUND)
	find_package(Boost REQUIRED)
endif (NOT BOOST_FOUND)

set(boost-test_LIBRARIES
	${BOOST_UNIT_TEST_FRAMEWORK_LIBRARY}
	CACHE INTERNAL "${PROJECT_NAME} libraries"
)

if (MSVC)
	ow_copy_file(${BOOST_LIBRARY_DIRS}/boost_unit_test_framework${BOOST_LIBRARIES_SUFFIX}.dll ${BUILD_DIR})
endif (MSVC)
