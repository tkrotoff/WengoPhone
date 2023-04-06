ow_create_shared_library(sipwrapper)

ow_use_public_libraries(
	coipmanager_base
	networkdiscovery
	pixertool
	owutil
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/CodecList.cpp
	src/EnumPhoneCallState.cpp
	src/EnumPhoneLineState.cpp
	src/EnumVideoQuality.cpp
	src/SipChatWrapper.cpp
	src/SipContactPresenceWrapper.cpp
	src/SipWrapper.cpp
	src/SipWrapperFactory.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/sipwrapper/SipChatWrapper.h
	include/sipwrapper/SipContactPresenceWrapper.h
	include/sipwrapper/SipWrapper.h
)

ow_create_project_binary()
