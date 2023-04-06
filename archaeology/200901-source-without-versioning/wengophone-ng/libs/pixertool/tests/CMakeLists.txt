ow_create_executable(pixertool-test)

ow_use_public_libraries(
	owcutil
	boost-test
	pixertool
	sdl_image
)

ow_add_sources(
	PixerToolTest.cpp
)

ow_create_project_binary(EXCLUDE_FROM_ALL)
