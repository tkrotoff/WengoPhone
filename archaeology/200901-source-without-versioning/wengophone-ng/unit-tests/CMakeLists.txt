ow_create_shared_library(unit-tests)

ow_add_sources(
	fake_main.c
)

ow_create_project_binary()

add_dependencies(${PROJECT_NAME}
	coipmanager-unit-tests
	coipmanager_base-unit-tests
	owsettings-unit-tests
	owutil-unit-tests
	webcam-unit-tests
)
