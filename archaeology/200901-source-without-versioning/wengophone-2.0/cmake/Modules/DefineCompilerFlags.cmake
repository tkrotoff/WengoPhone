# define system dependent compiler flags

include(CheckCXXCompilerFlag)

# with -fPIC
if(UNIX AND NOT WIN32)
  execute_process(
    COMMAND
      uname -m
    OUTPUT_VARIABLE MACHINE_TYPE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  MESSAGE("MACHINE_TYPE: ${MACHINE_TYPE}")
  if(MACHINE_TYPE MATCHES "x86_64")
    check_cxx_compiler_flag("-fPIC" WITH_FPIC)
    if(WITH_FPIC)
      ADD_DEFINITIONS(-fPIC)
    endif(WITH_FPIC)
  endif(MACHINE_TYPE MATCHES "x86_64")
endif(UNIX AND NOT WIN32)
