ow_create_plugin_library(PyCoIpManager)

ow_use_private_libraries(
	coipmanager_base
	coipmanager_threaded
	python
	sipwrapper
	owqtutil
	qtcore
	qtgui
	sip
)

# This include dir is put only in this script because of PyQt4:
# it does not use "standard" Qt4 include path:
# In qapplication.sip, PyQt4 includes qapplication.h whereas it
# should include QtGui/QApplication
find_package(Qt4 REQUIRED)
ow_add_private_include_dirs(
	${QT_QT_INCLUDE_DIR}
)

# List of source files got by typing:
# ls -l *.cpp | sed "s/.*\(sip.*$\)/\1/"
set(SOURCE_FILES
	sipPyCoIpManagerAIMAccount.cpp
	sipPyCoIpManagerAccount.cpp
	sipPyCoIpManagerAccountListHelper.cpp
	sipPyCoIpManagerChatMessage.cpp
	sipPyCoIpManagerCloneable.cpp
	sipPyCoIpManagerCoIpManagerConfig.cpp
	sipPyCoIpManagerCodecList.cpp
	sipPyCoIpManagerContact.cpp
	sipPyCoIpManagerContactListHelper.cpp
	sipPyCoIpManagerCountryList.cpp
	sipPyCoIpManagerDate.cpp
	sipPyCoIpManagerDesktopService.cpp
	sipPyCoIpManagerEnumAccountType.cpp
	sipPyCoIpManagerEnumChatStatusMessage.cpp
	sipPyCoIpManagerEnumChatTypingState.cpp
	sipPyCoIpManagerEnumConferenceCallState.cpp
	sipPyCoIpManagerEnumConnectionError.cpp
	sipPyCoIpManagerEnumConnectionState.cpp
	sipPyCoIpManagerEnumNatType.cpp
	sipPyCoIpManagerEnumNotificationType.cpp
	sipPyCoIpManagerEnumPhoneCallState.cpp
	sipPyCoIpManagerEnumPresenceState.cpp
	sipPyCoIpManagerEnumProxyAuthType.cpp
	sipPyCoIpManagerEnumSessionError.cpp
	sipPyCoIpManagerEnumSex.cpp
	sipPyCoIpManagerEnumSMSState.cpp
	sipPyCoIpManagerEnumTone.cpp
	sipPyCoIpManagerEnumUpdateSection.cpp
	sipPyCoIpManagerEnumVideoQuality.cpp
	sipPyCoIpManagerFile.cpp
	sipPyCoIpManagerFileReader.cpp
	sipPyCoIpManagerFileWriter.cpp
	sipPyCoIpManagerGTalkAccount.cpp
	sipPyCoIpManagerIAccount.cpp
	sipPyCoIpManagerICQAccount.cpp
	sipPyCoIpManagerIFile.cpp
	sipPyCoIpManagerIFileSession.cpp
	sipPyCoIpManagerIIMContact.cpp
	sipPyCoIpManagerImageSelector.cpp
	sipPyCoIpManagerIMContact.cpp
	sipPyCoIpManagerIMContactListHelper.cpp
	sipPyCoIpManagerITCoIpSessionManager.cpp
	sipPyCoIpManagerIdentifiable.cpp
	sipPyCoIpManagerInterface.cpp
	sipPyCoIpManagerJabberAccount.cpp
	sipPyCoIpManagerMSNAccount.cpp
	sipPyCoIpManagerNetworkProxy.cpp
	sipPyCoIpManagerNonCopyable.cpp
	sipPyCoIpManagerOWPicture.cpp
	sipPyCoIpManagerOscarAccount.cpp
	sipPyCoIpManagerPath.cpp
	sipPyCoIpManagerPeer.cpp
	sipPyCoIpManagerPixmapMerging.cpp
	sipPyCoIpManagerProfile.cpp
	sipPyCoIpManagerSipAccount.cpp
	sipPyCoIpManagerStreetAddress.cpp
	sipPyCoIpManagerStringList.cpp
	sipPyCoIpManagerTAccountManager.cpp
	sipPyCoIpManagerTCallSession.cpp
	sipPyCoIpManagerTCallSessionManager.cpp
	sipPyCoIpManagerTChatSession.cpp
	sipPyCoIpManagerTChatSessionManager.cpp
	sipPyCoIpManagerTCoIpManager.cpp
	sipPyCoIpManagerTCoIpModule.cpp
	sipPyCoIpManagerTConnectManager.cpp
	sipPyCoIpManagerTContactManager.cpp
	sipPyCoIpManagerTFileSessionManager.cpp
	sipPyCoIpManagerTIFileSession.cpp
	sipPyCoIpManagerTReceiveFileSession.cpp
	sipPyCoIpManagerTSendFileSession.cpp
	sipPyCoIpManagerTSession.cpp
	sipPyCoIpManagerTSMSSession.cpp
	sipPyCoIpManagerTSMSSessionManager.cpp
	sipPyCoIpManagerTUserProfileManager.cpp
	sipPyCoIpManagerTime.cpp
	sipPyCoIpManagerUserProfile.cpp
	sipPyCoIpManagerUserProfileFileStorage.cpp
	sipPyCoIpManagerWengoAccount.cpp
	sipPyCoIpManagerYahooAccount.cpp
	sipPyCoIpManagercmodule.cpp
	sipPyCoIpManagerpiximage.cpp
	sipPyCoIpManagerstdlist.cpp
	sipPyCoIpManagerstdmap.cpp
	sipPyCoIpManagerstdstring.cpp
)

set(PYTHON_COMMAND "python")
if (APPLE)
	# On Mac OS X, we want to use Python coming by the system which
	# is 2.3. Some machine could have installed more recent ones.
	set(PYTHON_COMMAND "python2.3")
endif(APPLE)

# Get parameters from ptqtconfig
add_custom_command(
	OUTPUT
		${SOURCE_FILES}
	COMMAND
		${PYTHON_COMMAND} generateFromSipFile.py ${SIP_EXECUTABLE}
	WORKING_DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}
)

ow_add_sources(
	${SOURCE_FILES}
)

ow_create_project_binary()
