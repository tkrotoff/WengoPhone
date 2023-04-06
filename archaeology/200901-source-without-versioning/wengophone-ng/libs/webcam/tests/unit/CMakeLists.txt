ow_create_executable(webcam-unit-tests)

ow_use_private_libraries(
	webcam
	boost-test
)

ow_add_private_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	main.cpp
)

if (MSVC80)
	# Solve link error atlthunk.lib when using Visual C++ 2005 Express
	ow_add_private_link_flags(
		/nodefaultlib:atlthunk.lib
	)
endif (MSVC80)

ow_create_project_binary()
