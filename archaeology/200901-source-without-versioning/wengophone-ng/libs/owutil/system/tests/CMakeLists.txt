ow_create_executable(owsystem-test-registerprotocol)

ow_use_private_libraries(
	owsystem
)

ow_add_sources(
	RegisterProtocol_test.cpp
)

ow_create_project_binary()


if (WIN32)

	ow_create_executable(owsystem-test-windowsregistry)

	ow_use_private_libraries(
		owsystem
	)

	ow_add_sources(
		WindowsRegistry_test.cpp
	)

	ow_create_project_binary()

endif (WIN32)
