ow_create_shared_library(coipmanager_threaded)

ow_use_public_libraries(
	# FIXME Should be private??
	coipmanager

	coipmanager_base
	qtgui
	sipwrapper
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/ITCoIpSessionManager.cpp
	src/TCoIpManager.cpp
	src/TCoIpModule.cpp

	src/datamanager/TAccountManager.cpp
	src/datamanager/TContactManager.cpp
	src/datamanager/TUserProfileManager.cpp

	src/connectmanager/TConnectManager.cpp

	src/session/TSession.cpp

	src/callsessionmanager/TCallSession.cpp
	src/callsessionmanager/TCallSessionManager.cpp

	src/chatsessionmanager/TChatSession.cpp
	src/chatsessionmanager/TChatSessionManager.cpp

	src/filesessionmanager/TIFileSession.cpp
	src/filesessionmanager/TFileSessionManager.cpp
	src/filesessionmanager/TReceiveFileSession.cpp
	src/filesessionmanager/TSendFileSession.cpp

	src/smssessionmanager/TSMSSession.cpp
	src/smssessionmanager/TSMSSessionManager.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/coipmanager_threaded/ITCoIpSessionManager.h
	include/coipmanager_threaded/TCoIpManager.h
	include/coipmanager_threaded/TCoIpModule.h

	include/coipmanager_threaded/datamanager/TAccountManager.h
	include/coipmanager_threaded/datamanager/TContactManager.h
	include/coipmanager_threaded/datamanager/TUserProfileManager.h

	include/coipmanager_threaded/connectmanager/TConnectManager.h

	include/coipmanager_threaded/session/TSession.h

	include/coipmanager_threaded/callsessionmanager/TCallSession.h
	include/coipmanager_threaded/callsessionmanager/TCallSessionManager.h

	include/coipmanager_threaded/chatsessionmanager/TChatSession.h
	include/coipmanager_threaded/chatsessionmanager/TChatSessionManager.h

	include/coipmanager_threaded/filesessionmanager/TFileSessionManager.h
	include/coipmanager_threaded/filesessionmanager/TReceiveFileSession.h
	include/coipmanager_threaded/filesessionmanager/TSendFileSession.h

	include/coipmanager_threaded/smssessionmanager/TSMSSession.h
	include/coipmanager_threaded/smssessionmanager/TSMSSessionManager.h
)

ow_create_project_binary()
