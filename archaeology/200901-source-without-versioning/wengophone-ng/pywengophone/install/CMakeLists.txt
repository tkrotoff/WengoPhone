set(CMAKE_CURRENT_SOURCE_DIR "${SOURCE_DIR}" CACHE INTERNAL "CMake source dir" FORCE)
set(CMAKE_SOURCE_DIR "${SOURCE_DIR}" CACHE INTERNAL "CMake source dir" FORCE)
set(CMAKE_BINARY_DIR "${BINARY_DIR}" CACHE INTERNAL "CMake binary dir" FORCE)

set(CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/owbuild
	${CMAKE_SOURCE_DIR}/owbuild/owbuild
)

# Add macros
include(${CMAKE_SOURCE_DIR}/owbuild/owbuild/CMakeLists.txt)

# Add options/default values...
include(${CMAKE_SOURCE_DIR}/DefineWengoOptions.cmake)
include(${CMAKE_SOURCE_DIR}/DefineInstallationPaths.cmake)

set(WORK_DIR ${CMAKE_SOURCE_DIR}/pywengophone/install)

if (APPLE)
	# CoIpManager SDK must be installed in /Library/Frameworks
	execute_process(
		COMMAND
			python setup.py
		WORKING_DIRECTORY
			${WORK_DIR}
	)

	execute_process(
	COMMAND
		rm -rf ${CMAKE_BINARY_DIR}/PyWengoPhone-0.2-r${SVN_REVISION}.dmg
	WORKING_DIRECTORY
		${WORK_DIR}
	)
	ow_hdiutil(PyWengoPhone ${WORK_DIR}/dist/PyWengoPhone.app ${CMAKE_BINARY_DIR}/PyWengoPhone-0.2-r${SVN_REVISION}.dmg)
endif (APPLE)

if (WIN32)
	# CoIpManager SDK must be installed in C:\ and built in MinSizeRel mode (because of missing python_d.dll otherwise)
	# PYTHONPATH must be set to where-CoIpManager-SDK-is-installed\lib
	execute_process(
		COMMAND
			python setup.py py2exe
		WORKING_DIRECTORY
			${WORK_DIR}
	)

	ow_zip_directory(${WORK_DIR}/dist/ ${CMAKE_BINARY_DIR}/PyWengoPhone-0.2-r${SVN_REVISION})
endif (WIN32)
