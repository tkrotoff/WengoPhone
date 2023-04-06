ow_create_project(boost-python)

ow_use_public_libraries(
	boost
	python
)

set(boost-python_LIBRARIES
	${python_LIBRARIES}
	${BOOST_PYTHON_LIBRARY}
	CACHE INTERNAL "${PROJECT_NAME} libraries"
)
