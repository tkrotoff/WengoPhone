ow_create_static_library(owphone)

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
	webcam
	netlib
	owsl
	tinyxml
	owphapiwrapper
	owbuildid
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/model/coipmanager/include
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
	model/config/EnumToolBarMode.cpp
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
	model/phoneline/PhoneLineStateAuthenticationError.cpp
	model/network/NetworkProxyDiscovery.cpp
	model/network/NetworkDiscovery.cpp
	model/network/NetworkObserver.cpp
	model/account/wengo/WengoAccount.cpp
	model/account/wengo/WengoAccountParser.cpp
	model/account/wengo/WengoAccountXMLSerializer.cpp
	model/account/SipAccount.cpp
	model/account/SipAccountXMLSerializer.cpp
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
	model/contactlist/ContactXMLSerializer1.cpp
	model/contactlist/ContactList.cpp
	model/contactlist/ContactListXMLSerializer.cpp
	model/contactlist/ContactListXMLSerializer1.cpp
	model/contactlist/ContactGroup.cpp
	model/contactlist/ContactListFileStorage.cpp
	model/contactlist/ContactListFileStorage1.cpp
	model/contactlist/ContactListRPCStorage.cpp
	model/contactlist/IMContactListHandler.cpp
	model/profile/Avatar.cpp
	model/profile/AvatarList.cpp
	model/profile/IMAccountManager.cpp
	model/profile/Profile.cpp
	model/profile/ProfileXMLSerializer.cpp
	model/profile/UserProfile.cpp
	model/profile/UserProfileHandler.cpp
	model/profile/UserProfileXMLSerializer.cpp
	model/profile/UserProfileFileStorage.cpp
	model/profile/UserProfileFileStorage1.cpp
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
	model/webservices/url/WsUrl.cpp
	model/classic/ClassicExterminator.cpp
	model/commandserver/CommandServer.cpp
	model/commandserver/CommandClient.cpp

	# FIXME: This part will be removed when coipmanager will be moved to libs
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
	##

	model/dtmf/Tone.cpp
	model/dtmf/DtmfTheme.cpp
	model/dtmf/DtmfThemeManager.cpp

	control/wenbox/CWenboxPlugin.cpp
	control/CWengoPhone.cpp
	control/chat/CChatHandler.cpp
	control/dtmf/CDtmfThemeManager.cpp
	control/phoneline/CPhoneLine.cpp
	control/phonecall/CPhoneCall.cpp
	control/phonecall/CConferenceCall.cpp
	control/profile/CUserProfile.cpp
	control/profile/CUserProfileHandler.cpp
	control/contactlist/CContactList.cpp
	control/webservices/sms/CSms.cpp
	control/webservices/softupdate/CSoftUpdate.cpp
	control/webservices/info/CWsInfo.cpp
	control/webservices/callforward/CWsCallForward.cpp
	control/history/CHistory.cpp

	presentation/PFactory.cpp
)

if (ENABLE_CRASHREPORT)
	if (WIN32 OR LINUX)
		message(STATUS "Crashreport activated")
		ow_use_private_libraries(
			owmemorydump
		)
		ow_add_private_definitions(
			-DENABLE_CRASHREPORT
		)
	else (WIN32 OR LINUX)
		message("Warning: ENABLE_CRASHREPORT is not supported on this platform")
	endif (WIN32 OR LINUX)
else (ENABLE_CRASHREPORT)
	message(STATUS "Crashreport deactivated")
endif (ENABLE_CRASHREPORT)

if (LINUX)
	if (WENGOPHONE_RESOURCEDIR_PATH)
		ow_add_private_definitions(
			-DOW_RESOURCEDIR='"${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}"'
		)
	endif (WENGOPHONE_RESOURCEDIR_PATH)
endif (LINUX)

if (PHAPI_CODEC_ILBC_SUPPORT)
	ow_add_private_definitions(
		-DENABLE_ILBC=1
	)
endif (PHAPI_CODEC_ILBC_SUPPORT)

ow_create_project_binary()

# TODO: test, xpcom, gtk?
subdirs(
	#presentation/gtk
	presentation/qt
	#presentation/xpcom
)
