if (NOT PYTHON_FOUND)
	find_package(PythonLibs REQUIRED)
	set(PYTHON_FOUND
		TRUE
	)
endif (NOT PYTHON_FOUND)

ow_create_project(python)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${PYTHON_DEBUG_LIBRARIES}
	)
else (CMAKE_BUILD_TYPE MATCHES Debug)
	ow_add_public_libraries(
		${PYTHON_LIBRARIES}
	)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

ow_add_public_include_dirs(
	${PYTHON_INCLUDE_PATH}
)
