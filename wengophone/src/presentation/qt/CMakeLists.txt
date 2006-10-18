project(qtwengophone)

include(${QT_USE_FILE})

subdirs(
  pics/emoticons
)

# needed include directories to build qtwengophone
# saves the variable in internal cache for later use
set(QTWENGOPHONE_INCLUDE_DIRS
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/toaster
  ${CMAKE_CURRENT_SOURCE_DIR}/profilebar

  ${OWPHONE_INCLUDE_DIRS}
  ${OWBUILDID_INCLUDE_DIRS}

  ${OWCUTIL_INCLUDE_DIRS}
  ${OWUTIL_INCLUDE_DIRS}
  ${OWTHREAD_INCLUDE_DIRS}
  ${OWQTUTIL_INCLUDE_DIRS}

  ${GAIM_INCLUDE_DIRS}
  ${PIXERTOOL_INCLUDE_DIRS}
  ${OWCURL_INCLUDE_DIRS}
  ${OWWEBCAM_INCLUDE_DIRS}
  ${OWBROWSER_INCLUDE_DIRS}
  ${OWTRAYICON_INCLUDE_DIRS}
  ${OWSOFTUPDATER_INCLUDE_DIRS}
  ${OWIDLE_INCLUDE_DIRS}
  ${OWPHAPIWRAPPER_INCLUDE_DIRS}
  ${OWMULTIIMWRAPPER_INCLUDE_DIRS}

  ${OSIP2_INCLUDE_DIRS}

  ${PORTAUDIO_INCLUDE_DIRS}
  ${GLIB2_INCLUDE_DIRS}
  ${FFMPEG_INCLUDE_DIRS}
  ${QT_INCLUDE_DIR}
  CACHE INTERNAL "qtwengophone include directories"
)

set(QTWENGOPHONE_EXECUTABLE
  qtwengophone
  CACHE INTERNAL "qtwengophone executable"
)

# qtwengophone dependencies
set(QTWENGOPHONE_LINK_LIBRARIES
  ${QTWENGOPHONE_EXECUTABLE}
  ${OWPHONE_LIBRARY}
  ${OWBUILDID_LIBRARY}

  ${OWCUTIL_LIBRARY}
  ${OWUTIL_LIBRARY}
  ${OWTHREAD_LIBRARY}
  ${OWQTUTIL_LIBRARY}

  ${GAIM_LIBRARY}
  ${PIXERTOOL_LIBRARY}
  ${OWCURL_LIBRARY}
  ${OWIDLE_LIBRARY}
  ${OWWEBCAM_LIBRARY}
  ${OWBROWSER_LIBRARY}
  ${OWTRAYICON_LIBRARY}
  ${OWSOFTUPDATER_LIBRARY}
  ${OWPHAPIWRAPPER_LIBRARY}
  ${OWMULTIIMWRAPPER_LIBRARY}

  ${OSIP2_LIBRARY}

  ${PORTAUDIO_LIBRARY}
  ${GLIB2_LIBRARY}
  ${AVUTIL_LIBRARY}
  ${AVCODEC_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTXML_LIBRARY}
  #${QT_QTUITOOLS_LIBRARY}
  QtUiTools

  gnutls
)


set(QTWENGOPHONE_DEFINITIONS
  -DPHAPIWRAPPER
  -DMULTIIMWRAPPER
)

#set(qtwengophone_SRCS
qt4_wrap_cpp(qtwengophone_SRCS
  QtWengoPhone.h
  QtConfigPanel.h
  QtDialpad.h
  QtAbout.h
  QtHttpProxyLogin.h
  QtIdle.h
  QtEnumIMProtocol.h
  QtLanguage.h
  QtToolBar.h
  QtSystray.h
  QtFactory.h
  QtUserProfilePresenceMenu.h
  QtWengoStyle.h
  QtBrowserWidget.h
  QtWebcamButton.h

  phoneline/QtPhoneLine.h

  phonecall/QtPhoneCall.h
  phonecall/QtVideo.h
  phonecall/QtVideoQt.h
  phonecall/QtContactCallList.h
  phonecall/QtContactCallListWidget.h
  phonecall/QtPhoneCallEventFilter.h

  toaster/QtChatToaster.h
  toaster/QtCallToaster.h
  toaster/QtToaster.h

  login/QtLogin.h

  profile/QtProfileDetails.h
  profile/QtUserProfile.h
  profile/QtUserProfileHandler.h

  imcontact/QtIMContactManager.h
  imcontact/QtAddIMContact.h
  imcontact/QtIMContactItem.h
  imcontact/QtSimpleIMContactManager.h

  imaccount/QtIMAccountItem.h
  imaccount/QtIMAccountManager.h
  imaccount/QtSimpleIMAccountManager.h
  imaccount/QtIMAccountSettings.h
  imaccount/QtIMAccountPlugin.h
  imaccount/QtMSNSettings.h
  imaccount/QtYahooSettings.h
  imaccount/QtAIMSettings.h
  imaccount/QtJabberSettings.h
  imaccount/QtGoogleTalkSettings.h

  contactlist/QtContactList.h
  contactlist/ContactGroupPopupMenu.h
  contactlist/QtContactWidget.h
  contactlist/QtContactTreeMouseFilter.h
  contactlist/QtContactManager.h
  contactlist/QtContactListManager.h
  contactlist/QtContact.h
  contactlist/QtTreeViewDelegate.h
  contactlist/QtContactPixmap.h
  contactlist/QtContactInfo.h
  contactlist/QtContactTreeKeyFilter.h
  contactlist/QtRenameGroup.h
  contactlist/QtConferenceAction.h
  contactlist/QtContactMenu.h

  conference/QtConferenceCallWidget.h

  chat/emoticons/QtEmoticonsWidget.h
  chat/emoticons/QtEmoticon.h
  chat/emoticons/QtEmoticonButton.h
  chat/emoticons/QtEmoticonsManager.h
  chat/chatroom/QtChatRoomInviteDlg.h
  chat/chatroom/QtChatRoomTreeWidgetItem.h
  chat/chatroom/QtChatRoomListWidgetItem.h
  chat/QtChatEditEventManager.h
  chat/QtChatTabWidget.h
  chat/QtChatAvatarFrame.h
  chat/QtChatAvatarWidget.h
  chat/QtChatHandler.h
  chat/QtChatWidget.h
  chat/QtChatWindow.h
  chat/QtChatEditWidget.h
  chat/QtChatHistoryWidget.h
  chat/QtChatEditActionBarWidget.h
  chat/QtWengoStyleBar.h
  chat/QtChatUtils.h

  webservices/sms/QtSms.h
  webservices/softupdate/QtSoftUpdate.h
  webservices/softupdate/QtBrowserSoftUpdate.h
  webservices/directory/QtWsDirectory.h
  webservices/directory/QtWsDirectoryContact.h

  callbar/QtCallBar.h
  callbar/QtPhoneComboBox.h

  statusbar/QtStatusBar.h

  config/QtISettings.h
  config/QtAdvancedConfig.h
  config/QtAccountSettings.h
  config/QtAdvancedSettings.h
  config/QtAudioSettings.h
  config/QtCallForwardSettings.h
  config/QtGeneralSettings.h
  config/QtNotificationSettings.h
  config/QtPrivacySettings.h
  config/QtVideoSettings.h
  config/QtLanguagesSettings.h
  config/QtWengoConfigDialog.h

  profilebar/QtEventWidget.h
  profilebar/QtIMProfileWidget.h
  profilebar/QtProfileBar.h
  profilebar/QtCreditWidget.h
  profilebar/QtIMAccountPresenceMenu.h

  history/QtHistoryWidget.h
  history/QtHistoryItem.h
  history/QtHistory.h

  filetransfer/QtFileTransfer.h
  filetransfer/QtFileTransferAcceptDialog.h
  filetransfer/QtFileTransferWidget.h
  filetransfer/QtFileTransferItem.h
  filetransfer/QtFileTransferDownloadItem.h
  filetransfer/QtFileTransferUploadItem.h
  filetransfer/QtFileTransferUpgradeDialog.h
)

set(qtwengophone_SRCS
  ${qtwengophone_SRCS}
  ../main.cpp
  QtWengoPhone.cpp
  QtConfigPanel.cpp
  QtDialpad.cpp
  QtWebcamButton.cpp
  QtAbout.cpp
  QtHttpProxyLogin.cpp
  QtIdle.cpp
  QtEnumIMProtocol.cpp
  QtLanguage.cpp
  QtToolBar.cpp
  QtSystray.cpp
  QtFactory.cpp
  QtUserProfilePresenceMenu.cpp
  QtWengoStyle.cpp
  QtBrowserWidget.cpp

  phoneline/QtPhoneLine.cpp

  phonecall/QtPhoneCall.cpp
  phonecall/QtVideo.cpp
  phonecall/QtVideoQt.cpp
  phonecall/QtContactCallList.cpp
  phonecall/QtContactCallListWidget.cpp
  phonecall/QtPhoneCallEventFilter.cpp

  toaster/QtChatToaster.cpp
  toaster/QtCallToaster.cpp
  toaster/QtToaster.cpp

  login/QtLogin.cpp

  imcontact/QtIMContactManager.cpp
  imcontact/QtAddIMContact.cpp
  imcontact/QtIMContactItem.cpp
  imcontact/QtSimpleIMContactManager.cpp

  profile/QtProfileDetails.cpp
  profile/QtUserProfile.cpp
  profile/QtUserProfileHandler.cpp

  imaccount/QtIMAccountItem.cpp
  imaccount/QtIMAccountManager.cpp
  imaccount/QtSimpleIMAccountManager.cpp
  imaccount/QtIMAccountSettings.cpp
  imaccount/QtIMAccountPlugin.cpp
  imaccount/QtMSNSettings.cpp
  imaccount/QtYahooSettings.cpp
  imaccount/QtAIMSettings.cpp
  imaccount/QtJabberSettings.cpp
  imaccount/QtGoogleTalkSettings.cpp

  wenbox/QtWenboxPlugin.cpp

  contactlist/QtContactList.cpp
  contactlist/ContactGroupPopupMenu.cpp
  contactlist/QtContactWidget.cpp
  contactlist/QtContactTreeMouseFilter.cpp
  contactlist/QtContactManager.cpp
  contactlist/QtContactListManager.cpp
  contactlist/QtContact.cpp
  contactlist/QtTreeViewDelegate.cpp
  contactlist/QtContactPixmap.cpp
  contactlist/QtContactInfo.cpp
  contactlist/QtContactTreeKeyFilter.cpp
  contactlist/QtRenameGroup.cpp
  contactlist/QtConferenceAction.cpp
  contactlist/QtContactMenu.cpp

  conference/QtConferenceCallWidget.cpp

  chat/emoticons/QtEmoticonsWidget.cpp
  chat/emoticons/QtEmoticon.cpp
  chat/emoticons/QtEmoticonButton.cpp
  chat/emoticons/QtEmoticonsManager.cpp
  chat/chatroom/QtChatRoomInviteDlg.cpp
  chat/chatroom/QtChatRoomTreeWidgetItem.cpp
  chat/chatroom/QtChatRoomListWidgetItem.cpp
  chat/QtChatTabWidget.cpp
  chat/QtChatAvatarFrame.cpp
  chat/QtChatAvatarWidget.cpp
  chat/QtChatHandler.cpp
  chat/QtChatWidget.cpp
  chat/QtChatEditEventManager.cpp
  chat/QtChatWindow.cpp
  chat/QtChatEditWidget.cpp
  chat/QtChatHistoryWidget.cpp
  chat/QtChatEditActionBarWidget.cpp
  chat/QtWengoStyleBar.cpp
  chat/QtChatUtils.cpp

  webservices/sms/QtSms.cpp
  webservices/softupdate/QtSoftUpdate.cpp
  webservices/softupdate/QtBrowserSoftUpdate.cpp
  webservices/directory/QtWsDirectory.cpp
  webservices/directory/QtWsDirectoryContact.cpp

  callbar/QtCallBar.cpp
  callbar/QtPhoneComboBox.cpp

  statusbar/QtStatusBar.cpp

  config/QtAdvancedConfig.cpp
  config/QtAccountSettings.cpp
  config/QtAdvancedSettings.cpp
  config/QtAudioSettings.cpp
  config/QtCallForwardSettings.cpp
  config/QtGeneralSettings.cpp
  config/QtNotificationSettings.cpp
  config/QtPrivacySettings.cpp
  config/QtVideoSettings.cpp
  config/QtLanguagesSettings.cpp
  config/QtWengoConfigDialog.cpp

  profilebar/QtEventWidget.cpp
  profilebar/QtIMProfileWidget.cpp
  profilebar/QtCreditWidget.cpp
  profilebar/QtIMAccountPresenceMenu.cpp
  profilebar/QtProfileBar.cpp

  history/QtHistoryWidget.cpp
  history/QtHistoryItem.cpp
  history/QtHistory.cpp

  filetransfer/QtFileTransfer.cpp
  filetransfer/QtFileTransferAcceptDialog.cpp
  filetransfer/QtFileTransferWidget.cpp
  filetransfer/QtFileTransferItem.cpp
  filetransfer/QtFileTransferDownloadItem.cpp
  filetransfer/QtFileTransferUploadItem.cpp
  filetransfer/QtFileTransferUpgradeDialog.cpp
)

if (WIN32)
  #if (MSVC)
  #  qt4_add_resources(qtwengophone_SRCS
  #    win32/wengophone.rc
  #  )
  #else (MSVC)
  #endif (MSVC)
  set(QTWENGOPHONE_INCLUDE_DIRS
    ${QTWENGOPHONE_INCLUDE_DIRS}
    ${OWMEMORYDUMP_INCLUDE_DIRS}
  )

  set(QTWENGOPHONE_LINK_LIBRARIES
    ${QTWENGOPHONE_LINK_LIBRARIES}
    ${OWMEMORYDUMP_LIBRARY}
  )
endif (WIN32)

if (UNIX AND NOT WIN32)
  if (APPLE)
    qt4_wrap_cpp(
      chat/QtCloseChatTabWidget.h
      macosx/QtMacApplication.h
    )

    set(qtwengophone_SRCS
      ${qtwengophone_SRCS}
      chat/QtCloseChatTabWidget.cpp
      macosx/QtMacApplication.cpp
    )
  else (APPLE)
    set(QTWENGOPHONE_INCLUDE_DIRS
      ${QTWENGOPHONE_INCLUDE_DIRS}
      ${X11_INCLUDE_DIR}
    )

    set(QTWENGOPHONE_LINK_LIBRARIES
      ${QTWENGOPHONE_LINK_LIBRARIES}
      ${X11_LIBRARIES}
    )

    option(WITH_HW_ACCEL "Build with Xv hardware acceleration support" ON)
    if (WITH_HW_ACCEL)
      if (X11_XV_FOUND)
        set(QTWENGOPHONE_LINK_LIBRARIES
          ${QTWENGOPHONE_LINK_LIBRARIES}
          ${X11_Xv_LIB}
        )
        set(QTWENGOPHONE_DEFINITIONS
          ${QTWENGOPHONE_DEFINITIONS}
          -DXV_HWACCEL
        )

        qt4_wrap_cpp(qtwengophone_SRCS
          phonecall/XVWindow.h
          phonecall/QtVideoXV.h
        )

      set(qtwengophone_SRCS
        ${qtwengophone_SRCS}
        phonecall/XVWindow.cpp
        phonecall/QtVideoXV.cpp
      )
    endif (X11_XV_FOUND)
  endif (WITH_HW_ACCEL)
endif (APPLE)
endif (UNIX AND NOT WIN32)

qt4_wrap_ui(qtwengophone_SRCS
  WengoPhoneWindow.ui
  WengoPhoneWindowConfigPanel.ui
  AboutWindow.ui
  HttpProxyLoginWindow.ui
  DialpadWidget.ui

  login/LoginWindow.ui

  imaccount/IMAccountTemplate.ui
  imaccount/AIMSettings.ui
  imaccount/GoogleTalkSettings.ui
  imaccount/IMAccountManager.ui
  imaccount/SimpleIMAccountManager.ui
  imaccount/JabberSettings.ui
  imaccount/MSNSettings.ui
  imaccount/YahooSettings.ui

  profile/ProfileDetails.ui

  imcontact/IMContactManager.ui
  imcontact/AddIMContact.ui
  imcontact/SimpleIMContactManager.ui

  webservices/directory/WsDirectory.ui
  webservices/directory/WsDirectoryContact.ui
  webservices/sms/SmsWindow.ui
  webservices/softupdate/SoftUpdateWindow.ui
  webservices/softupdate/BrowserSoftUpdateWindow.ui

  conference/ConferenceCallWidget.ui

  config/AccountSettings.ui
  config/AdvancedConfigWindow.ui
  config/GeneralSettings.ui
  config/AdvancedSettings.ui
  config/AudioSettings.ui
  config/CallForwardSettings.ui
  config/LanguagesSettings.ui
  config/ManageBlockedUsersSettings.ui
  config/NotificationSettings.ui
  config/PrivacySettings.ui
  config/VideoSettings.ui
  config/WengoConfigDialog.ui

  chat/ChatMainWindow.ui
  chat/CloseChatTabWidget.ui
  chat/ChatWidget.ui
  chat/UserFrame.ui
  chat/AvatarFrame.ui
  chat/AvatarWidget.ui

  contactlist/RenameGroupDialog.ui
  contactlist/ContactWidget.ui
  contactlist/ContactList.ui

  profilebar/CreditWidget.ui
  profilebar/EventWidget.ui
  profilebar/IMProfileWidget.ui

  toaster/QtChatToaster.ui
  toaster/QtCallToaster.ui

  phonecall/PhoneCallWidget.ui
  phonecall/VideoWindow.ui

  filetransfer/AcceptDialog.ui
  filetransfer/FileTransferWidget.ui
  filetransfer/FileTransferItem.ui
  filetransfer/UpgradeDialog.ui

  history/HistoryWidget.ui

  statusbar/StatusBarWidget.ui
)

qt4_add_resources(qtwengophone_SRCS
  wengophone.qrc
)

macro_generate_qt_qm_files(
  ${CMAKE_CURRENT_SOURCE_DIR}/lang
  qtwengophone_SRCS
  ${DATA_INSTALL_DIR}/lang
)

include_directories(
  ${QTWENGOPHONE_INCLUDE_DIRS}
)

add_definitions(
  ${QTWENGOPHONE_DEFINITIONS}
)

add_executable(${QTWENGOPHONE_EXECUTABLE} ${qtwengophone_SRCS})

target_link_libraries(${QTWENGOPHONE_LINK_LIBRARIES})

if (APPLE)
  macro_add_compile_flags(${QTWENGOPHONE_EXECUTABLE}
    "-x objective-c++"
  )

  macro_add_link_flags(${QTWENGOPHONE_EXECUTABLE}
    "-framework CoreAudio"
    "-framework AudioToolbox"
    "-framework AudioUnit"
    "-framework Cocoa"
    "-framework Carbon"
  )
endif (APPLE)

install(
  TARGETS
    ${QTWENGOPHONE_EXECUTABLE}
  DESTINATION
    ${BIN_INSTALL_DIR}
)

