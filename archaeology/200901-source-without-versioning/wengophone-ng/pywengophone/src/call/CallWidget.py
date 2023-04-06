# -*- coding: utf-8 -*-
#
# WengoPhone, a voice over Internet phone
# Copyright (C) 2004-2007 Wengo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# @author Philippe Bernery
# @author Ovidiu Ciule

from PhoneCallWidget_ui import Ui_PhoneCallWidget
from QVideoWidget import QVideoWidget

from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging, sys

log = logging.getLogger("PyWengoPhone.CallWidget")

class CallWidget(QWidget):
    def __init__(self, tCoIpManager, callManager):
        QWidget.__init__(self, None)

        self.__tCoIpManager = tCoIpManager
        self.__tCallSession = None
        self.__callManager = callManager
        self.__testedVideo = False
        self.__localRealVideo = False
        self.__remoteRealVideo = False

        ## Init GUI
        self.__ui = Ui_PhoneCallWidget()
        self.__ui.setupUi(self)
        self.initGuiSignals()
        self.__ui.dialpadButton.setEnabled(False)
        self.__ui.smileysButton.setEnabled(False)
        self.__ui.addContactButton.setEnabled(False)
        self.setFixedSize(300, 330)
        ####
        callManager.callWidgets.append(self)

    def initGuiSignals(self):
        connect(self.__ui.hangUpButton, QtCore.SIGNAL("clicked()"), self.callSessionEnd)
        connect(self.__ui.acceptButton, QtCore.SIGNAL("clicked()"), self.acceptClicked)
        connect(self.__ui.rejectButton, QtCore.SIGNAL("clicked()"), self.rejectClicked)

    def callContact(self, contact):
        """
        Calls a contact.
        """
        self.__tCallSession = self.__prepareSession()
        if self.__tCallSession != None:
            self.__tCallSession.addContact(contact)
            self.__tCallSession.start()
        self.__ui.nickNameLabel.setText(contact.getDisplayName())
        self.setWindowTitle("Calling "+str(contact.getDisplayName()))
        self.showRingingGUI()
        # TODO show the proper avatar, when contact.getAvatar is implemented
        self.initAvatarLabel(QPixmap(":/pics/avatars/default-avatar.png"))

    def callNumber(self, number):
        """
        Calls a number.
        """
        self.__tCallSession = self.__prepareSession()
        contact = Contact()
        phoneNumber = number
        # TODO: create proper contact
        contact.addIMContact(IMContact(EnumAccountType.AccountTypeSIP,
            str(phoneNumber)))
        self.__tCallSession.addContact(contact)
        self.__tCallSession.start()
        self.__ui.nickNameLabel.setText(str(number))
        self.setWindowTitle("Calling "+str(number))
        self.showRingingGUI()

    def showRingingGUI(self):
        self.__ui.buttonStack.setCurrentIndex(0)
        self.__ui.acceptButton.hide()
        self.__ui.rejectButton.setText("Cancel and Close")

    def receiveCall(self, tCallSession):
        """
        Answers an incoming call
        """
        # Show the avatar page
        self.__ui.labelStack.setCurrentIndex(1)
        # Show the Call accept/reject buttons
        self.__ui.buttonStack.setCurrentIndex(0)
        # Display 'Incoming Call'
        self.__ui.statusLabel.setText("Incoming Call")
        # TODO: only enable video if necessary
        tCallSession.enableVideo(True)
        self.initSessionSignals(tCallSession)
        self.__tCallSession = tCallSession
        self.__ui.nickNameLabel.setText(self.getNickname(tCallSession))
        self.__ui.nickNameLabel.setWindowTitle(self.getNickname(tCallSession))

    def getNickname(self, tCallSession):
        contacts = tCallSession.getContactList()
        if len(contacts)>0:
            return contact[0].getDisplayName()
        return "Unknown caller"

    def acceptClicked(self):
        """
        User clicked the Accept Call button
        Start the callSession
        """
        # Start the session
        if self.__tCallSession!=None:
            self.__tCallSession.start()

    def rejectClicked(self):
        """
        User clicked the reject call button
        """
        self.close()

    def __prepareSession(self):
        """
        Create and init a TCallSession, then return it.
        """
        tCallSession = self.__tCoIpManager.getTCallSessionManager().createTCallSession()
        # TODO: only enable video if necessary
        tCallSession.enableVideo(True)
        self.initSessionSignals(tCallSession)
        return tCallSession

    def initSessionSignals(self, tCallSession):
        """
        Connect signals to a call session
        """
        connect(tCallSession,
            SIGNAL("videoFrameReceivedSignal(QImage, QImage)"),
            self.videoFrameReceivedSlot)
        connect(tCallSession,
            SIGNAL("phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)"),
            self.phoneCallStateChangedSlot)

    def disconnectSessionSignals(self, tCallSession):
        QObject.disconnect(self.__tCallSession,
            QtCore.SIGNAL("videoFrameReceivedSignal(QImage, QImage)"),
            self.videoFrameReceivedSlot)
        QObject.disconnect(self.__tCallSession,
            QtCore.SIGNAL("phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)"),
            self.phoneCallStateChangedSlot)

    def closeEvent(self, event):
        log.debug("closeEvent()")
        self.callSessionEnd()
        QDialog.closeEvent(self, event)

    def phoneCallStateChangedSlot(self, state):
        log.debug("phoneCallStateChangedSlot: state="+EnumPhoneCallState.toString(state))
        try:
            {
                EnumPhoneCallState.PhoneCallStateUnknown: self.callSessionEnd,
                EnumPhoneCallState.PhoneCallStateClosed:  self.callSessionEnd,
                EnumPhoneCallState.PhoneCallStateError:   self.callSessionError,

                EnumPhoneCallState.PhoneCallStateDialing: self.callSessionDialing,
                EnumPhoneCallState.PhoneCallStateRinging: self.callSessionRinging,
                EnumPhoneCallState.PhoneCallStateTalking: self.callSessionTalking,
            }[state]()
        except KeyError:
            pass

    def callSessionEnd(self):
        self.__ui.statusLabel.setText("Closed")
        if self.__tCallSession != None:
            self.disconnectSessionSignals(self.__tCallSession)
            if self.canBeClosed(self.__tCallSession):
               self.__tCallSession.stop()
            self.__tCallSession = None
        self.showCallEndedGUI()

    def showCallEndedGUI(self):
        self.__ui.buttonStack.setCurrentIndex(1)
        self.__ui.hangUpButton.setEnabled(False)
        self.__ui.holdButton.setEnabled(False)
        self.__ui.rejectButton.setText("Close")

    def callSessionError(self):
        self.__ui.statusLabel.setText("Error")
        self.showCallEndedGUI()

    def callSessionTalking(self):
        # Show in-talk buttons
        self.__ui.statusLabel.setText("Talking")
        self.__ui.buttonStack.setCurrentIndex(1)
        self.__ui.hangUpButton.setEnabled(True)
        self.__ui.holdButton.setEnabled(False)
        if self.__tCallSession.isVideoEnabled():
            self.__ui.labelStack.setCurrentWidget(self.__ui.videoPage)

    def callSessionRinging(self):
        self.__ui.statusLabel.setText("Ringing")

    def callSessionDialing(self):
        self.__ui.statusLabel.setText("Dialing")

    def canBeClosed(self, tCallSession):
        state = tCallSession.getState()
        return not state in [EnumPhoneCallState.PhoneCallStateClosed,
            EnumPhoneCallState.PhoneCallStateError,
            EnumPhoneCallState.PhoneCallStateUnknown]

    def videoFrameReceivedSlot(self, remoteVideoFrame, localVideoFrame):
        #log.debug("videoFrameReceivedSlot()")
        if not self.__testedVideo:
            self.__testedVideo = True
            self.__remoteRealVideo = CallWidget.isRealWebcamVideoFrame(remoteVideoFrame)
            print "remote: " + str(self.__remoteRealVideo)
            self.__localRealVideo = CallWidget.isRealWebcamVideoFrame(localVideoFrame)
            print "local: " + str(self.__localRealVideo)
            # If no video at all
            if not self.__localRealVideo and not self.__remoteRealVideo:
                self.__ui.labelStack.setCurrentWidget(self.__ui.avatarPage)
            # If no local video
            if not self.__localRealVideo:
                #self.__ui.localWebcamLabel.hide()
                self.__ui.localWebcamLabel.setText("No Video")
            # If no remote video
            if not self.__remoteRealVideo:
                #self.__ui.remoteWebcamLabel.hide()
                self.__ui.remoteWebcamLabel.setText("No Video")
        if self.__localRealVideo:
            self.__ui.localWebcamLabel.setImage(localVideoFrame)

        if self.__remoteRealVideo:
            self.__ui.remoteWebcamLabel.setImage(remoteVideoFrame)

    def closeEvent(self, event):
        log.debug("close()")
        self.callSessionEnd()
        try:
            self.__callManager.callWidgets.remove(self)
        except Error: pass
        event.accept()

    def initAvatarLabel(self, pixmap):
        if pixmap.hasAlpha():
            # Paint avatar over a background if it's transparent (doing so for
            # full opaque images looks ugly)
            bg = QPixmap(":/pics/phonecall/avatar_background.png")
            painter = QPainter(bg)
            painter.drawPixmap(
                int((bg.width() - pixmap.width()) / 2),
                int((bg.height() - pixmap.height()) / 2),
                pixmap)
            painter.end()
            self.__ui.avatarLabel.setPixmap(bg)
        else:
            self.__ui.avatarLabel.setPixmap(pixmap)
        self.__ui.labelStack.setCurrentWidget(self.__ui.avatarPage)

    def isRealWebcamVideoFrame(image):
        """
        Helper function to determine if image is a real webcam or not:
        If the remote user has no webcam, then wifo will send us a yuv image filled
        with NOWEBCAM_FILL_VALUE bytes.
        """
        # This value must be identical to the one in wifo/phapi/phmedia-video.c
        NOWEBCAM_FILL_VALUE = '\x82'
        if image==None or image.bits()==None or image.numBytes()==0:
            return False
        imgData = image.bits().asstring(image.numBytes())
        for i in range(0, len(imgData), 4):
            pixel = imgData[i+1]
            # If pixel != 128 +/- 5
            if abs(ord(pixel)-ord(NOWEBCAM_FILL_VALUE))>5:
                return True
        return False
    isRealWebcamVideoFrame = staticmethod(isRealWebcamVideoFrame)
