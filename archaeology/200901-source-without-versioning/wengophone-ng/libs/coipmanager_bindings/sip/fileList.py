"""Permanent file list, do not overwrite!
Sip files can be automatically generated from these header files.
Please read README and README.SipGeneratorScript for more information
Warning: not all files can be automatically processed. If this is the case,
add it here, commented out, with an explanation, as a warning. If a file is
commented without explanation, it probably means it is not needed (yet)"""
tasks = []
tasks.append (["coipmanager_threaded",
	"libs/coipmanager_threaded/include/coipmanager_threaded/TCoIpManager.h",
        # Constructor was removed manually
	"libs/coipmanager_threaded/include/coipmanager_threaded/TCoIpModule.h",
	# Does not define a class
	#"libs/coipmanager_threaded/include/coipmanager_threaded/tcoipmanagerdll.h",
        # Wrong inheritance
	#"libs/coipmanager_threaded/include/coipmanager_threaded/session/TSession.h",
	#"libs/coipmanager_threaded/include/coipmanager_threaded/connectmanager/TConnectManager.h",
	#"libs/coipmanager_threaded/include/coipmanager_threaded/datamanager/TContactManager.h",
	# Private method getAccountManager returns AccountManager, SIP error
	#"libs/coipmanager_threaded/include/coipmanager_threaded/datamanager/TAccountManager.h",
	#"libs/coipmanager_threaded/include/coipmanager_threaded/datamanager/TUserProfileManager.h",
	#"libs/coipmanager_threaded/include/coipmanager_threaded/callsessionmanager/TCallSessionManager.h",
	#"libs/coipmanager_threaded/include/coipmanager_threaded/callsessionmanager/TCallSession.h",
	# Needs special handling for ChatMessageList
	#"libs/coipmanager_threaded/include/coipmanager_threaded/chatsessionmanager/TChatSession.h",
	#"libs/coipmanager_threaded/include/coipmanager_threaded/chatsessionmanager/TChatSessionManager.h"])
tasks.append (["coipmanager_base",
        "libs/coipmanager_base/include/coipmanager_base/contact/ContactList.h",
        "libs/coipmanager_base/include/coipmanager_base/account/AccountList.h",
        "libs/coipmanager_base/include/coipmanager_base/imcontact/IIMContact.h",
        "libs/coipmanager_base/include/coipmanager_base/EnumPresenceState.h",
        "libs/coipmanager_base/include/coipmanager_base/peer/Peer.h",
	# a private function returns IMContactList::iterator, and SIP does't
	# recognize it as a valid type
        #"libs/coipmanager_base/include/coipmanager_base/contact/Contact.h",
        "libs/coipmanager_base/include/coipmanager_base/account/IAccount.h",
        "libs/coipmanager_base/include/coipmanager_base/account/Account.h",
        "libs/coipmanager_base/include/coipmanager_base/account/GTalkAccount.h",
        "libs/coipmanager_base/include/coipmanager_base/account/JabberAccount.h",
        # Multiple Inheritance, SIP only supports one parent, so second parent's methods must be added in the .sip file
        #"libs/coipmanager_base/include/coipmanager_base/account/SipAccount.h",
        "libs/coipmanager_base/include/coipmanager_base/account/YahooAccount.h",
        "libs/coipmanager_base/include/coipmanager_base/account/WengoAccount.h",
        "libs/coipmanager_base/include/coipmanager_base/userprofile/UserProfile.h",
        "libs/coipmanager_base/include/coipmanager_base/profile/Profile.h",
        "libs/coipmanager_base/include/coipmanager_base/contact/ContactGroupList.h",
        "libs/coipmanager_base/include/coipmanager_base/storage/UserProfileFileStorage.h",
	# Contains functions definitions, which are hard to remove without a C++ parser :-(
        #"libs/coipmanager_base/include/coipmanager_base/profile/StreetAddress.h",
        "libs/coipmanager_base/include/coipmanager_base/imcontact/IMContactList.h",
        "libs/coipmanager_base/include/coipmanager_base/imcontact/IMContact.h",
        "libs/coipmanager_base/include/coipmanager_base/EnumAccountType.h",
        "libs/coipmanager_base/include/coipmanager_base/EnumChatTypingState.h",
        "libs/coipmanager_base/include/coipmanager_base/profile/EnumSex.h",
        "libs/coipmanager_base/include/coipmanager_base/EnumConnectionState.h"])

tasks.append (["coipmanager",
#        "libs/coipmanager/include/coipmanager/CoIpManagerPluginLoader.h",
#        "libs/coipmanager/include/coipmanager/ICoIpManagerPlugin.h",
#        "libs/coipmanager/include/coipmanager/CoIpModule.h",
#        "libs/coipmanager/include/coipmanager/AccountMutator.h",
	# If generated automatically, inherits AutomaticSettings, which generates SIP errors
        #"libs/coipmanager/include/coipmanager/CoIpManagerConfig.h",
        "libs/coipmanager/include/coipmanager/CoIpManagerUser.h",
	# a private function returns a custom typedef; a private copy constructor must be provided, otherwise SIP adds a public one
        #"libs/coipmanager/include/coipmanager/CoIpManager.h",
	"libs/coipmanager/include/coipmanager/ICoIpManager.h",
	"libs/coipmanager/include/coipmanager/ICoIpSessionManager.h",
        "libs/coipmanager/include/coipmanager/CoIpManagerSaver.h",
        # Should not be exported
        #"libs/coipmanager/include/coipmanager/session/Session.h",
        # Should not be exported
        #"libs/coipmanager/include/coipmanager/session/ISession.h",
        "libs/coipmanager/include/coipmanager/connectmanager/EnumConnectionError.h",
	# Contains an external C function which must be removed
        #"libs/coipmanager/include/coipmanager/connectmanager/IConnectManagerPlugin.h",
        "libs/coipmanager/include/coipmanager/connectmanager/ConnectManager.h",
        "libs/coipmanager/include/coipmanager/connectmanager/IConnectPlugin.h",
        "libs/coipmanager/include/coipmanager/connectmanager/ConnectedState.h",
        "libs/coipmanager/include/coipmanager/connectmanager/DisconnectedState.h",
        "libs/coipmanager/include/coipmanager/connectmanager/ConnectingState.h",
        "libs/coipmanager/include/coipmanager/notification/EnumNotificationType.h",
        "libs/coipmanager/include/coipmanager/notification/Notification.h",
        "libs/coipmanager/include/coipmanager/presencemanager/contact/ContactPresenceManager.h",
        "libs/coipmanager/include/coipmanager/presencemanager/account/AccountPresenceManager.h",
        "libs/coipmanager/include/coipmanager/datamanager/UserProfileManager.h",
        "libs/coipmanager/include/coipmanager/datamanager/EnumUpdateSection.h",
        "libs/coipmanager/include/coipmanager/datamanager/ContactGroupManager.h",
	# A private function returns an unsupported type
        #"libs/coipmanager/include/coipmanager/datamanager/ContactManager.h",
	# A private function returns an unsupported type
        #"libs/coipmanager/include/coipmanager/datamanager/AccountManager.h",
        "libs/coipmanager/include/coipmanager/syncmanager/SyncManager.h",
        "libs/coipmanager/include/coipmanager/syncmanager/contact/ContactSyncManager.h",
        "libs/coipmanager/include/coipmanager/callsessionmanager/CallSessionManager.h",
        "libs/coipmanager/include/coipmanager/callsessionmanager/CallSession.h",
	# Constructor must be private, otherwise SIP tries to instance this interface
        #"libs/coipmanager/include/coipmanager/callsessionmanager/ICallSessionPlugin.h",
	# Contains an external C function which must be removed
	# Constructor must be private, otherwise SIP tries to instance this interface
        #"libs/coipmanager/include/coipmanager/callsessionmanager/ICallSessionManagerPlugin.h",
        "libs/coipmanager/include/coipmanager/filesessionmanager/SendFileSession.h",
        "libs/coipmanager/include/coipmanager/filesessionmanager/FileSessionManager.h",
	# Contains an external C function which must be removed
	# Constructor must be private, otherwise SIP tries to instance this interface
        #"libs/coipmanager/include/coipmanager/filesessionmanager/IFileSessionManagerPlugin.h",
        "libs/coipmanager/include/coipmanager/filesessionmanager/ISendFileSessionPlugin.h",
        "libs/coipmanager/include/coipmanager/filesessionmanager/ReceiveFileSession.h",
        "libs/coipmanager/include/coipmanager/filesessionmanager/IReceiveFileSessionPlugin.h",
        "libs/coipmanager/include/coipmanager/filesessionmanager/IFileSession.h",
        "libs/coipmanager/include/coipmanager/chatsessionmanager/EnumChatStatusMessage.h",
        "libs/coipmanager/include/coipmanager/chatsessionmanager/ChatSessionManager.h",
	# Needs special handlings for ChatMessageList
        #"libs/coipmanager/include/coipmanager/chatsessionmanager/ChatSession.h",
	# Constructor must be private, otherwise SIP tries to instance this interface
        #"libs/coipmanager/include/coipmanager/chatsessionmanager/IChatSessionPlugin.h",
	# Contains an external C function which must be removed
	# Constructor must be private, otherwise SIP tries to instance this interface
        #"libs/coipmanager/include/coipmanager/chatsessionmanager/IChatSessionManagerPlugin.h",
        "libs/coipmanager/include/coipmanager/chatsessionmanager/ChatMessage.h"])


tasks.append (["owutil",
	# include path transformed into relative
        #"libs/owutil/util/NonCopyable.h",
        "libs/owutil/util/Date.h",
        "libs/owutil/util/Identifiable.h",
        "libs/owutil/util/OWPicture.h",
        #"libs/owutil/util/Interface.h",
	# !!! Must be written manually, because StringList is a special type
        #"libs/owutil/util/StringList.h",
        "libs/owutil/util/Cloneable.h",
	# Needs special %If directive        
        #"libs/owutil/util/Path.h",
        "libs/owutil/util/File.h"
	])


tasks.append (["sipwrapper",
        "libs/sipwrapper/include/sipwrapper/CodecList.h",
        "libs/sipwrapper/include/sipwrapper/EnumConferenceCallState.h",
        "libs/sipwrapper/include/sipwrapper/EnumTone.h",
        "libs/sipwrapper/include/sipwrapper/EnumPhoneCallState.h",
	# contains static constants, which SIP doesn't like
        #"libs/sipwrapper/include/sipwrapper/SipWrapper.h",
        #"libs/sipwrapper/include/sipwrapper/WebcamVideoFrame.h",
        "libs/sipwrapper/include/sipwrapper/EnumVideoQuality.h",
        #"libs/sipwrapper/include/sipwrapper/SipWrapperFactory.h",
        "libs/sipwrapper/include/sipwrapper/EnumPhoneLineState.h"])

tasks.append (["networkdiscovery",
#       "libs/networkdiscovery/include/networkdiscovery/NetworkProxyDiscovery.h"
	# Uses type defined in netlib.h
#       "libs/networkdiscovery/include/networkdiscovery/EnumProxyAuthType.h"
       "libs/networkdiscovery/include/networkdiscovery/EnumNatType.h"
	])

# C module, .sip must be handwritten
#tasks.append (["pixertool",
#        "libs/pixertool/include/pixertool/pixertool.h"])
