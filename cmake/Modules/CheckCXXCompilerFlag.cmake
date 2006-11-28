# - CHECK_CXX_COMPILER_FLAG(flag result)
# Check whether the compiler supports a given flag
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


include(CheckCXXSourceCompiles)

macro (CHECK_CXX_COMPILER_FLAG flag result)
	set(SAFE_CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
	set(CMAKE_REQUIRED_DEFINITIONS "${flag}")
	CHECK_CXX_SOURCE_COMPILES("int main(){return 0;}" ${result})
	set(CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")
endmacro (CHECK_CXX_COMPILER_FLAG)
