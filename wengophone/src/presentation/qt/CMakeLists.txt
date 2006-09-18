project(qtwengophone)

include(${QT_USE_FILE})

# needed include directories to build qtwengophone
# saves the variable in internal cache for later use
set(QTWENGOPHONE_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OWPHONE_INCLUDE_DIRS}
  ${BUILDID_INCLUDE_DIRS}
  ${QTUTIL_INCLUDE_DIRS}
  ${WEBCAM_INCLUDE_DIRS}
  ${OWBROWSER_INCLUDE_DIRS}
  ${TRAYICON_INCLUDE_DIRS}
  ${SOFTUPDATER_INCLUDE_DIRS}
  ${OWIDLE_INCLUDE_DIRS}
  ${OWPHAPIWRAPPER_INCLUDE_DIRS}
  ${OWMULTIIMWRAPPER_INCLUDE_DIRS}
  ${OWTRAYICON_INCLUDE_DIRS}
  ${QT_INCLUDE_DIR}
  CACHE INTERNAL "qtwengophone include directories"
)

set(QTWENGOPHONE_LIBRARY
  qtwengophone
  CACHE INTERNAL "qtwengophone library"
)

# qtwengophone lib and dependencies
# saves the variable in internal cache for later use
set(QTWENGOPHONE_LIBRARIES
  ${QTWENGOPHONE_LIBRARY}
  ${OWPHONE_LIBRARIES}
  ${BUILDID_LIBRARIES}
  ${QTUTIL_LIBRARIES}
  ${WEBCAM_LIBRARIES}
  ${OWBROWSER_LIBRARIES}
  ${TRAYICON_LIBRARIES}
  ${SOFTUPDATER_LIBRARIES}
  ${OWIDLE_LIBRARIES}
  ${OWPHAPIWRAPPER_LIBRARIES}
  ${OWMULTIIMWRAPPER_LIBRARIES}
  ${OWTRAYICON_LIBRARIES}
  CACHE INTERNAL "qtwengophone libraries"
)


set(QTWENGOPHONE_DEFINITIONS
  -DPHAPIWRAPPER
  -DMULTIIMWRAPPER
)

set(qtwengophone_SRCS
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
  webservices/subscribe/QtSubscribe.cpp
  webservices/subscribe/QtSubscribe2.cpp
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
)

qt4_automoc(${qtwengophone_SRCS})

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
  webservices/subscribe/SubscribeWengo1.ui
  webservices/subscribe/SubscribeWengo2.ui
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
  #chat/chatroom/ChatRoomInviteWidget.ui

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

  history/HistoryWidget.ui

  statusbar/StatusBarWidget.ui
)

if (WIN32)
  if (MSVC)
    # TODO win32/wengophone.rc
  else (MSVC)
  endif (MSVC)
endif (WIN32)

if (UNIX AND NOT WIN32)
  if (APPLE)
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

    set(QTWENGOPHONE_LIBRARIES
      ${QTWENGOPHONE_LIBRARIES}
      ${X11_LIBRARIES}
    )

    option(WITH_HW_ACCEL "Build with Xv hardware acceleration support" ON)
    if (WITH_HW_ACCEL)
      if (X11_XV_FOUND)
        set(QTWENGOPHONE_LIBRARIES
          ${QTWENGOPHONE_LIBRARIES}
          ${X11_Xv_LIB}
        }
        set(QTWENGOPHONE_DEFINITIONS
          ${QTWENGOPHONE_DEFINITIONS}
          -DXV_HWACCEL
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

include_directories(
  ${QTWENGOPHONE_INCLUDE_DIRS}
)

add_definitions(
  ${QTWENGOPHONE_DEFINITIONS}
)


add_executable(qtwengophone ${qtwengophone_SRCS})

target_link_libraries(${QTWENGOPHONE_LIBRARIES})

