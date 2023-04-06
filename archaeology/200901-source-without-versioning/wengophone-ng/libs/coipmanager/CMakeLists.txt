ow_create_shared_library(coipmanager)

#Used to generate package and install directories
set(COIPMANAGER_BUNDLE_NAME "CoIpManager" CACHE INTERNAL "CoIpManager SDK bundle name")
set(COIPMANAGER_VERSION "0.2" CACHE INTERNAL "CoIpManager version")

ow_use_public_libraries(
	coipmanager_base
	sipwrapper
	owutil
)

ow_use_private_libraries(
	owsettings
	owshlibloader
	sound
	webcam
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_private_definitions(
	-DCOIPMANAGER_PLUGIN_DLL
	-DCOIPMANAGER_PLUGIN_PATH="${CMAKE_INSTALL_PREFIX}/${COIPMANAGER_BUNDLE_NAME}-${COIPMANAGER_VERSION}/lib/"
)

ow_add_sources(
	src/AccountMutator.cpp
	src/CoIpManager.cpp
	src/CoIpManagerConfig.cpp
	src/CoIpManagerPluginLoader.cpp
	src/CoIpManagerSaver.cpp
	src/CoIpManagerUser.cpp
	src/CoIpModule.cpp
	src/ICoIpManager.cpp
	src/ICoIpManagerPlugin.cpp
	src/ICoIpSessionManager.cpp
	src/ICoIpSessionManagerPlugin.cpp

	src/session/ISession.cpp
	src/session/Session.cpp

	src/callsessionmanager/CallSession.cpp
	src/callsessionmanager/CallSessionManager.cpp
	src/callsessionmanager/ICallSessionPlugin.cpp
	src/callsessionmanager/ICallSessionManagerPlugin.cpp

	src/chatsessionmanager/ChatSession.cpp
	src/chatsessionmanager/ChatMessage.cpp
	src/chatsessionmanager/ChatSessionManager.cpp
	src/chatsessionmanager/IChatSessionPlugin.cpp
	src/chatsessionmanager/IChatSessionManagerPlugin.cpp

	src/connectmanager/ConnectedState.cpp
	src/connectmanager/ConnectingState.cpp
	src/connectmanager/ConnectManager.cpp
	src/connectmanager/DisconnectedState.cpp
	src/connectmanager/DisconnectingState.cpp
	src/connectmanager/EnumConnectionError.cpp
	src/connectmanager/IConnectPlugin.cpp
	src/connectmanager/IConnectManagerPlugin.cpp
	src/connectmanager/IConnectState.cpp

	src/datamanager/AccountManager.cpp
	src/datamanager/ContactManager.cpp
	src/datamanager/ContactGroupManager.cpp
	src/datamanager/UserProfileManager.cpp

	src/filesessionmanager/FileSessionManager.cpp
	src/filesessionmanager/IFileSession.cpp
	src/filesessionmanager/IFileSessionManagerPlugin.cpp
	src/filesessionmanager/IReceiveFileSession.cpp
	src/filesessionmanager/IReceiveFileSessionPlugin.cpp
	src/filesessionmanager/ISendFileSession.cpp
	src/filesessionmanager/ISendFileSessionPlugin.cpp
	src/filesessionmanager/ReceiveFileSession.cpp
	src/filesessionmanager/SendFileSession.cpp

	src/initplugin/ICoIpManagerInitPlugin.cpp

	src/notification/Notification.cpp

	src/presencemanager/account/AccountPresenceManager.cpp
	src/presencemanager/account/IAccountPresenceManagerPlugin.cpp

	src/presencemanager/contact/ContactPresenceManager.cpp
	src/presencemanager/contact/IContactPresenceManagerPlugin.cpp

	src/smssessionmanager/ISMSSession.cpp
	src/smssessionmanager/ISMSSessionManagerPlugin.cpp
	src/smssessionmanager/ISMSSessionPlugin.cpp
	src/smssessionmanager/SMSSession.cpp
	src/smssessionmanager/SMSSessionManager.cpp

	src/syncmanager/ISyncManagerPlugin.cpp
	src/syncmanager/SyncManager.cpp
	src/syncmanager/contact/ContactSyncManager.cpp
	src/syncmanager/contact/IContactSyncManagerPlugin.cpp
	src/syncmanager/contact/IIMContactSyncManagerPlugin.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/coipmanager/AccountMutator.h
	include/coipmanager/CoIpModule.h
	include/coipmanager/CoIpManagerSaver.h
	include/coipmanager/ICoIpManager.h

	include/coipmanager/session/ISession.h
	include/coipmanager/session/Session.h

	include/coipmanager/callsessionmanager/ICallSession.h
	include/coipmanager/callsessionmanager/CallSession.h
	include/coipmanager/callsessionmanager/CallSessionManager.h
	include/coipmanager/callsessionmanager/ICallSessionPlugin.h
	include/coipmanager/callsessionmanager/ICallSessionManagerPlugin.h

	include/coipmanager/chatsessionmanager/IChatSession.h
	include/coipmanager/chatsessionmanager/ChatSession.h
	include/coipmanager/chatsessionmanager/ChatSessionManager.h
	include/coipmanager/chatsessionmanager/IChatSessionPlugin.h
	include/coipmanager/chatsessionmanager/IChatSessionManagerPlugin.h

	include/coipmanager/connectmanager/ConnectManager.h
	include/coipmanager/connectmanager/IConnectPlugin.h

	include/coipmanager/datamanager/AccountManager.h
	include/coipmanager/datamanager/UserProfileManager.h
	include/coipmanager/datamanager/ContactManager.h
	include/coipmanager/datamanager/ContactGroupManager.h

	include/coipmanager/filesessionmanager/FileSessionManager.h
	include/coipmanager/filesessionmanager/IFileSessionManagerPlugin.h
	include/coipmanager/filesessionmanager/IReceiveFileSessionPlugin.h
	include/coipmanager/filesessionmanager/ISendFileSessionPlugin.h
	include/coipmanager/filesessionmanager/ReceiveFileSession.h
	include/coipmanager/filesessionmanager/SendFileSession.h

	include/coipmanager/notification/Notification.h
	
	include/coipmanager/presencemanager/account/AccountPresenceManager.h

	include/coipmanager/presencemanager/contact/ContactPresenceManager.h
	include/coipmanager/presencemanager/contact/IContactPresenceManagerPlugin.h

	include/coipmanager/smssessionmanager/ISMSSessionPlugin.h
	include/coipmanager/smssessionmanager/SMSSession.h

	include/coipmanager/syncmanager/contact/ContactSyncManager.h
	include/coipmanager/syncmanager/contact/IContactSyncManagerPlugin.h
	include/coipmanager/syncmanager/contact/IIMContactSyncManagerPlugin.h
)

ow_create_project_binary()

subdirs(
	tests
)
