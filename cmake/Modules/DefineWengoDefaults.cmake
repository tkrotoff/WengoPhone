# Required cmake version
cmake_minimum_required(VERSION 2.4.3)

# Always include srcdir and builddir in include path
# This saves typing ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} in
# about every subdir
# since cmake 2.4.0
set(CMAKE_INCLUDE_CURRENT_DIR OFF)

# Put the include dirs which are in the source or build tree
# before all other include dirs, so the headers in the sources
# are prefered over the already installed ones
# since cmake 2.4.1
set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# Use colored output
# since cmake 2.4.0
set(CMAKE_COLOR_MAKEFILE ON)

# Define the generic version of the libraries here
set(GENERIC_LIB_VERSION "2.0.0")
set(GENERIC_LIB_SOVERSION "2")

# Set the default build type to debug
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE
		Debug
		CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE
	)
endif (NOT CMAKE_BUILD_TYPE)
