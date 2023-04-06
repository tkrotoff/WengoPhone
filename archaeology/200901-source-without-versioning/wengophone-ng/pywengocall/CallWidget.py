"""
  WengoPhone, a voice over Internet phone
  Copyright (C) 2004-2007  Wengo
 
  This program is free software you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

"""
  Test program for PyCoIpManager
  
  @author Ovidiu Ciule
  @author Philippe Bernery
"""

from SafeConnect import *
from CallWidget_ui import Ui_CallWidget
from QVideoWidget import QVideoWidget
from PyQt4 import QtCore, QtGui
from PyCoIpManager import *
import logging, sys

log = logging.getLogger("PyWengoCall")


class CallWidget(QtGui.QWidget):

  def __init__(self, tCoIpManager):
    self._tCoIpManager = tCoIpManager
    self._tCallSession = None
    self._ctImages = 0

    # Init GUI
    QtGui.QWidget.__init__(self, None)
    self._ui = Ui_CallWidget()
    self._ui.setupUi(self)

    connect(self._ui.callButton,   QtCore.SIGNAL("clicked()"), self.callButtonClicked)
    connect(self._ui.hangUpButton, QtCore.SIGNAL("clicked()"), self.hangUpButtonClicked)

    self._ui.hangUpButton.setEnabled(False)
    self._ui.phoneNumberLineEdit.setText("sip:335@voip.wengo.fr")

  def callButtonClicked(self):
    self._tCallSession = self._tCoIpManager.getTCallSessionManager().createTCallSession()

    connect(self._tCallSession,
     QtCore.SIGNAL("videoFrameReceivedSignal2(QImage, QImage)"),
     self.videoFrameReceivedSlot)
    connect(self._tCallSession,
     QtCore.SIGNAL("phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)"),
     self.phoneCallStateChangedSlot)

    contact = Contact()
    contact.addIMContact(IMContact(EnumAccountType.AccountTypeWengo,
     str(self._ui.phoneNumberLineEdit.text())))
    self._tCallSession.addContact(contact)
    self._tCallSession.enableVideo(True)
    self._tCallSession.start()
    self._ctImages = 0

  def hangUpButtonClicked(self):
   self.callSessionEnd()

  def phoneCallStateChangedSlot(self, state):
    try:
      {EnumPhoneCallState.PhoneCallStateUnknown: self.callSessionEnd,
       EnumPhoneCallState.PhoneCallStateClosed:  self.callSessionEnd,
       EnumPhoneCallState.PhoneCallStateError:   self.callSessionEnd,

       EnumPhoneCallState.PhoneCallStateTalking: self.callSessionStart,
       EnumPhoneCallState.PhoneCallStateDialing: self.callSessionStart,
       EnumPhoneCallState.PhoneCallStateRinging: self.callSessionStart
      }[state]()
    except KeyError: pass

  def callSessionEnd(self):
    if (self._tCallSession!=None):
      QtCore.QObject.disconnect(self._tCallSession,
       QtCore.SIGNAL("videoFrameReceivedSignal(piximage *, piximage *)"),
       self.videoFrameReceivedSlot)
      disconnect(self._tCallSession,
       QtCore.SIGNAL("phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)"),
       self.phoneCallStateChangedSlot)

      self._tCallSession.stop()
      self._tCallSession = None
      self._ui.hangUpButton.setEnabled(False)
      self._ui.callButton.setEnabled(True)

  def callSessionStart(self):
    self._ui.hangUpButton.setEnabled(True)
    self._ui.callButton.setEnabled(False)

  def videoFrameReceivedSlot(self, remoteVideoFrame, localVideoFrame):
    self._ui.localWebcamLabel.setImage(localVideoFrame)
    self._ui.remoteWebcamLabel.setImage(remoteVideoFrame)

  def closeEvent(self, event):
    self.callSessionEnd()
    event.accept()
    self.emit(QtCore.SIGNAL("closing()"))
