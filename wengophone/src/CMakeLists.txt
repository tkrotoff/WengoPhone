ow_create_project(owphone)

ow_use_public_libraries(
	owsettings
	owsystem
	owhttp
	owserialization
	owserializer
	owsocket
	owimwrapper
	owsipwrapper
	owmultiimwrapper
	owwenbox
	owsound
)

ow_use_private_libraries(
	owwebcam
	netlib
	tinyxml
	owphapiwrapper
	owbuildid
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/model/coipmanager/include
	${CMAKE_CURRENT_SOURCE_DIR}/model/coipmanager/chatsessionmanager/include
	${CMAKE_CURRENT_SOURCE_DIR}/model/coipmanager/filesessionmanager/include
)

ow_add_sources(
	model/WengoPhone.cpp
	model/SipCallbacks.cpp
	model/config/Config.cpp
	model/config/ConfigImporter.cpp
	model/config/ConfigXMLSerializer.cpp
	model/config/ConfigManager.cpp
	model/config/ConfigManagerFileStorage.cpp
	model/config/LanguageList.cpp
	model/config/StartupSettingListener.cpp
	model/config/CommandLineParser.cpp
	model/connect/Connect.cpp
	model/connect/ConnectHandler.cpp
	model/chat/ChatHandler.cpp
	model/wenbox/WenboxPlugin.cpp
	model/wenbox/EnumWenboxStatus.cpp
	model/phonecall/SipAddress.cpp
	model/phonecall/PhoneCall.cpp
	model/phonecall/PhoneCallState.cpp
	model/phonecall/PhoneCallStateClosed.cpp
	model/phonecall/PhoneCallStateUnknown.cpp
	model/phonecall/PhoneCallStateDialing.cpp
	model/phonecall/PhoneCallStateError.cpp
	model/phonecall/PhoneCallStateHold.cpp
	model/phonecall/PhoneCallStateIncoming.cpp
	model/phonecall/PhoneCallStateTalking.cpp
	model/phonecall/PhoneCallStateResumed.cpp
	model/phonecall/PhoneCallStateRinging.cpp
	model/phonecall/PhoneCallStateMissed.cpp
	model/phonecall/ConferenceCall.cpp
	model/phonecall/ConferenceCallParticipant.cpp
	model/phoneline/PhoneLine.cpp
	model/phoneline/PhoneLineStateUnknown.cpp
	model/phoneline/PhoneLineStateProgress.cpp
	model/phoneline/PhoneLineStateOk.cpp
	model/phoneline/PhoneLineStateClosed.cpp
	model/phoneline/PhoneLineStateTimeout.cpp
	model/phoneline/PhoneLineStateServerError.cpp
	model/network/NetworkProxyDiscovery.cpp
	model/network/NetworkDiscovery.cpp
	model/network/NetworkObserver.cpp
	model/account/wengo/WengoAccount.cpp
	model/account/wengo/WengoAccountParser.cpp
	model/account/wengo/WengoAccountXMLSerializer.cpp
	#model/account/LocalNetworkAccount.cpp
	model/account/SipAccount.cpp
	model/presence/Presence.cpp
	model/presence/PresenceHandler.cpp
	model/presence/MyPresenceStateOnline.cpp
	model/presence/MyPresenceStateOffline.cpp
	model/presence/MyPresenceStateAway.cpp
	model/presence/MyPresenceStateDoNotDisturb.cpp
	model/presence/MyPresenceStateInvisible.cpp
	model/contactlist/Contact.cpp
	model/contactlist/ContactProfile.cpp
	model/contactlist/ContactPresenceState.cpp
	model/contactlist/ContactPresenceStateAway.cpp
	model/contactlist/ContactPresenceStateOnline.cpp
	model/contactlist/ContactPresenceStateOffline.cpp
	model/contactlist/ContactPresenceStateDoNotDisturb.cpp
	model/contactlist/ContactPresenceStateUnknown.cpp
	model/contactlist/ContactPresenceStateUnavailable.cpp
	model/contactlist/ContactXMLSerializer.cpp
	model/contactlist/ContactList.cpp
	model/contactlist/ContactListXMLSerializer.cpp
	model/contactlist/ContactGroup.cpp
	model/contactlist/ContactListFileStorage.cpp
	model/contactlist/ContactListRPCStorage.cpp
	model/contactlist/IMContactListHandler.cpp
	model/profile/Avatar.cpp
	model/profile/AvatarList.cpp
	model/profile/Profile.cpp
	model/profile/ProfileXMLSerializer.cpp
	model/profile/UserProfile.cpp
	model/profile/UserProfileHandler.cpp
	model/profile/UserProfileXMLSerializer.cpp
	model/profile/UserProfileFileStorage.cpp
	model/profile/EnumSex.cpp
	model/profile/StreetAddress.cpp
	model/profile/StreetAddressXMLSerializer.cpp
	model/history/History.cpp
	model/history/HistoryMemento.cpp
	model/history/HistoryMementoCollection.cpp
	model/webservices/WengoWebService.cpp
	model/webservices/sms/WsSms.cpp
	model/webservices/info/WsInfo.cpp
	model/webservices/softupdate/WsSoftUpdate.cpp
	model/webservices/callforward/WsCallForward.cpp
	model/webservices/directory/WsDirectory.cpp
	model/webservices/url/WsUrl.cpp
	model/classic/ClassicExterminator.cpp
	model/commandserver/CommandServer.cpp

	# FIXME: This part will be removed when coipmanager wille be moved to libs
	model/coipmanager/src/CoIpManager.cpp
	model/coipmanager/src/Session.cpp

	model/coipmanager/filesessionmanager/src/FileSessionManager.cpp
	model/coipmanager/filesessionmanager/src/IFileSession.cpp
	model/coipmanager/filesessionmanager/src/IFileSessionManager.cpp
	model/coipmanager/filesessionmanager/src/IReceiveFileSession.cpp
	model/coipmanager/filesessionmanager/src/ISendFileSession.cpp
	model/coipmanager/filesessionmanager/src/ReceiveFileSession.cpp
	model/coipmanager/filesessionmanager/src/SendFileSession.cpp
	model/coipmanager/filesessionmanager/implementation/phapi/src/PhApiSendFileSession.cpp
	model/coipmanager/filesessionmanager/implementation/phapi/src/PhApiReceiveFileSession.cpp
	model/coipmanager/filesessionmanager/implementation/phapi/src/PhApiFileSessionManager.cpp

	model/coipmanager/chatsessionmanager/src/ChatSession.cpp
	model/coipmanager/chatsessionmanager/src/ChatSessionManager.cpp
	model/coipmanager/chatsessionmanager/src/IChatSession.cpp
	model/coipmanager/chatsessionmanager/src/IChatSessionManager.cpp
	##

	model/dtmf/Tone.cpp
	model/dtmf/DtmfTheme.cpp
	model/dtmf/DtmfThemeManager.cpp

	control/wenbox/CWenboxPlugin.cpp
	control/CWengoPhone.cpp
	control/chat/CChatHandler.cpp
	control/phoneline/CPhoneLine.cpp
	control/phonecall/CPhoneCall.cpp
	control/phonecall/CConferenceCall.cpp
	control/profile/CUserProfile.cpp
	control/profile/CUserProfileHandler.cpp
	control/contactlist/CContactList.cpp
	control/webservices/sms/CSms.cpp
	control/webservices/softupdate/CSoftUpdate.cpp
	control/webservices/info/CWsInfo.cpp
	control/webservices/directory/CWsDirectory.cpp
	control/webservices/callforward/CWsCallForward.cpp
	control/history/CHistory.cpp
	control/dtmf/CDtmfThemeManager.cpp

	presentation/PFactory.cpp
)

if (WIN32)
	ow_use_private_libraries(
		owmemorydump
	)
endif (WIN32)

if (UNIX AND NOT WIN32)
	option(WITH_RESOURCEDIR "Use ${DATA_INSTALL_DIR} as hardcoded resouce path" ON)
	if (WITH_RESOURCEDIR)
		ow_add_private_definitions(
			-DOW_RESOURCEDIR="\\"${DATA_INSTALL_DIR}\\""
		)
	endif (WITH_RESOURCEDIR)
endif (UNIX AND NOT WIN32)

ow_create_static_library()

# TODO: test, xpcom, gtk?
subdirs(
	#presentation/gtk
	presentation/qt
	#presentation/xpcom
)
