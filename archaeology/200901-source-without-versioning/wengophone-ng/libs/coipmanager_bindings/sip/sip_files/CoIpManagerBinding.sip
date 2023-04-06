%Module PyCoIpManager 0
%License /Type="GPL"/
%Platforms {WINDOWS_PLATFORM LINUX_PLATFORM MACOSX_PLATFORM}

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip

// Standard type
%Include stdlist.sip
%Include stdmap.sip
%Include stdstring.sip

// owutil
%Include Cloneable.sip
%Include CountryList.sip
%Include Date.sip
%Include File.sip
%Include Identifiable.sip
%Include Interface.sip
%Include NonCopyable.sip
%Include OWPicture.sip
%Include Path.sip
%Include StringList.sip
%Include Time.sip

// qtutil
%Include DesktopService.sip
%Include ImageSelector.sip
%Include PixmapMerging.sip

// Pixertool
%Include pixertool.sip

// CoIpManager base
%Include Account.sip
%Include AccountList.sip
%Include AIMAccount.sip
%Include Contact.sip
%Include ContactGroupList.sip
%Include ContactList.sip
%Include EnumAccountType.sip
%Include EnumChatTypingState.sip
%Include EnumConnectionState.sip
%Include EnumPresenceState.sip
%Include EnumSex.sip
%Include EnumUpdateSection.sip
%Include GTalkAccount.sip
%Include IAccount.sip
%Include ICQAccount.sip
%Include IIMContact.sip
%Include IMContact.sip
%Include IMContactList.sip
%Include JabberAccount.sip
%Include MSNAccount.sip
%Include OscarAccount.sip
%Include Peer.sip
%Include Profile.sip
%Include SipAccount.sip
%Include WengoAccount.sip
%Include YahooAccount.sip
%Include StreetAddress.sip
%Include UserProfile.sip
%Include UserProfileFileStorage.sip

// Sipwrapper
%Include CodecList.sip
%Include EnumConferenceCallState.sip
%Include EnumPhoneCallState.sip
%Include EnumTone.sip
%Include EnumVideoQuality.sip

// CoIpManager
%Include ChatMessage.sip
%Include CoIpManagerConfig.sip
%Include EnumChatStatusMessage.sip
%Include EnumConnectionError.sip
%Include EnumNotificationType.sip
%Include EnumSessionError.sip
%Include EnumSMSState.sip
%Include IFileSession.sip

// TCoIpManager
%Include ITCoIpSessionManager.sip
%Include TAccountManager.sip
%Include TCallSession.sip
%Include TCallSessionManager.sip
%Include TChatSession.sip
%Include TChatSessionManager.sip
%Include TCoIpManager.sip
%Include TCoIpModule.sip
%Include TConnectManager.sip
%Include TContactManager.sip
%Include TFileSessionManager.sip
%Include TIFileSession.sip
%Include TReceiveFileSession.sip
%Include TSendFileSession.sip
%Include TSession.sip
%Include TSMSSession.sip
%Include TSMSSessionManager.sip
%Include TUserProfileManager.sip

// networkdiscovery
%Include EnumProxyAuthType.sip
%Include EnumNatType.sip
%Include NetworkProxyDiscovery.sip
