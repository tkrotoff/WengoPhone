ow_create_static_library(owsipwrapper)

#subdirs(
	#src/phapi
#)

ow_use_public_libraries(
	owutil
	owpixertool
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/SipWrapperFactory.cpp
	src/EnumPhoneCallState.cpp
	src/EnumPhoneLineState.cpp
	src/EnumNatType.cpp
	src/EnumVideoQuality.cpp
	src/CodecList.cpp
)

ow_create_binary()
