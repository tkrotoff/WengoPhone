# Define system dependent compiler flags

# With -fPIC
if (UNIX AND NOT WIN32)
	if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
		ow_check_cxx_compiler_flag("-fPIC" WITH_FPIC)
		if (WITH_FPIC)
			add_definitions(-fPIC)
		endif (WITH_FPIC)
	endif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
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

# Enable warnings
if (MSVC)
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else (MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (MSVC)
