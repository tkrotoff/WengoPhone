ow_create_executable(owsettings-unit-tests)

ow_use_private_libraries(
	owsettings
	boost-test
)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	main.cpp
)

ow_create_project_binary()
