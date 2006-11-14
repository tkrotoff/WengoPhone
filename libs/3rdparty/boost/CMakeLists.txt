ow_create_project(boost)

ow_use_public_libraries(
	BOOST
)

ow_add_public_definitions(
	-DBOOST_ALL_DYN_LINK
	${BOOST_LIB_DIAGNOSTIC_DEFINITIONS}
)

if (MSVC)
	ow_add_public_definitions(
		/wd4251
		/wd4275
		/wd4675
	)
endif (MSVC)
