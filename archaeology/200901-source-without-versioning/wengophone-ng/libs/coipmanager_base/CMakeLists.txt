ow_create_shared_library(coipmanager_base)

ow_use_public_libraries(
	networkdiscovery
	owserialization
	owserializer
	owutil
	tinyxml
	qtcore
)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ow_add_sources(
	src/EnumAccountType.cpp
	src/EnumChatTypingState.cpp
	src/EnumConnectionState.cpp
	src/EnumPresenceState.cpp

	src/account/Account.cpp
	src/account/AccountList.cpp
	src/account/AIMAccount.cpp
	src/account/GTalkAccount.cpp
	src/account/IAccount.cpp
	src/account/IAXAccount.cpp
	src/account/ICQAccount.cpp
	src/account/JabberAccount.cpp
	src/account/MSNAccount.cpp
	src/account/OscarAccount.cpp
	src/account/SipAccount.cpp
	src/account/WengoAccount.cpp
	src/account/YahooAccount.cpp

	src/contact/Contact.cpp
	src/contact/ContactList.cpp

	src/imcontact/GenericIMContact.cpp
	src/imcontact/IIMContact.cpp
	src/imcontact/IMContact.cpp
	src/imcontact/IMContactList.cpp
	src/imcontact/SipIMContact.cpp
	src/imcontact/WengoIMContact.cpp

	src/peer/Peer.cpp
	src/peer/SipPeer.cpp

	src/profile/EnumSex.cpp
	src/profile/Profile.cpp
	src/profile/StreetAddress.cpp

	src/serializer/AccountListXMLSerializer.cpp
	src/serializer/AccountXMLSerializer.cpp
	src/serializer/ContactGroupListXMLSerializer.cpp
	src/serializer/ContactListXMLSerializer.cpp
	src/serializer/ContactXMLSerializer.cpp
	src/serializer/IAccountXMLSerializer.cpp
	src/serializer/IMContactXMLSerializer.cpp
	src/serializer/OscarAccountXMLSerializer.cpp
	src/serializer/PeerXMLSerializer.cpp
	src/serializer/ProfileXMLSerializer.cpp
	src/serializer/SipAccountXMLSerializer.cpp
	src/serializer/SipPeerXMLSerializer.cpp
	src/serializer/StreetAddressXMLSerializer.cpp
	src/serializer/UserProfileXMLSerializer.cpp
	src/serializer/WengoAccountXMLSerializer.cpp

	src/storage/AccountListFileStorage.cpp
	src/storage/ContactGroupListFileStorage.cpp
	src/storage/ContactListFileStorage.cpp
	src/storage/UserProfileFileStorage.cpp

	src/userprofile/UserProfile.cpp
)

qt4_wrap_cpp(${PROJECT_NAME}_SRCS
	include/coipmanager_base/profile/Profile.h
)

ow_create_project_binary()

subdirs(
	tests
)
