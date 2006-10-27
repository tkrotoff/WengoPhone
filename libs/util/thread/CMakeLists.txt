ow_create_project(owthread)

ow_use_public_libraries(
	owutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${BOOST_INCLUDE_DIRS}
)

ow_add_private_libraries(
	${BOOST_THREAD_LIBRARY}
)

#ow_add_public_definitions(
#	-DOWTHREAD_DLL
#)

#ow_add_private_definitions(
#	-DOWTHREAD_DLL
#	-DBUILD_OWTHREAD_DLL
#)

ow_add_sources(
	src/Timer.cpp
	src/Thread.cpp
)

ow_create_static_library()
