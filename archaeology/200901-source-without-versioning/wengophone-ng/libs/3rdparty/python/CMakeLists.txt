if (NOT PYTHON_FOUND)
	find_package(PythonLibs REQUIRED)
	set(PYTHON_FOUND
		TRUE
	)
endif (NOT PYTHON_FOUND)

ow_create_project(python)

if (NOT PYTHON_LIBRARIES)
	if (WIN32)
		# There is a bug inside FindPythonLibs under Windows
		set(PYTHON_LIBRARIES
			C:/Python25/libs/python25.lib
		)
	endif (WIN32)
endif (NOT PYTHON_LIBRARIES)

ow_add_public_libraries(
	${PYTHON_LIBRARIES}
)

if (NOT PYTHON_INCLUDE_PATH)
	if (WIN32)
		# There is a bug inside FindPythonLibs under Windows
		set(PYTHON_INCLUDE_PATH
			C:/Python25/include
			C:/Python24/include
		)
	endif (WIN32)
endif (NOT PYTHON_INCLUDE_PATH)

ow_add_public_include_dirs(
	${PYTHON_INCLUDE_PATH}
)
