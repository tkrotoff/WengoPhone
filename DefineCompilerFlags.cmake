# Define system dependent compiler flags

include(CheckCXXCompilerFlag)

# With -fPIC
if (UNIX AND NOT WIN32)
	execute_process(
		COMMAND
			uname -m
		OUTPUT_VARIABLE MACHINE_TYPE
		OUTPUT_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (MACHINE_TYPE MATCHES "x86_64")
		check_cxx_compiler_flag("-fPIC" WITH_FPIC)
		if (WITH_FPIC)
			add_definitions(-fPIC)
		endif (WITH_FPIC)
	endif (MACHINE_TYPE MATCHES "x86_64")
endif (UNIX AND NOT WIN32)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	# No MSVCRT.LIB linking under Visual C++ when in debug mode
	if (MSVC)
		set(CMAKE_EXE_LINKER_FLAGS
			"${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB"
		)
		set(CMAKE_SHARED_LINKER_FLAGS
			"${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB"
		)
		set(CMAKE_MODULE_LINKER_FLAGS
			"${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB"
		)
	endif (MSVC)

	# Defines DEBUG when in debug mode
	add_definitions(-DDEBUG)
endif (CMAKE_BUILD_TYPE MATCHES Debug)
