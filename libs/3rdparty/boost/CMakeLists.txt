ow_create_project(boost)

ow_use_public_libraries(
	BOOST
)

ow_add_public_definitions(
	-DBOOST_ALL_DYN_LINK
)

if (MSVC)
	ow_add_public_definitions(
		/wd4251
		/wd4275
		/wd4675
	)
endif (MSVC)
