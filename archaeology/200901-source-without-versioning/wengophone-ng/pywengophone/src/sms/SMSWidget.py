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

from SMSWidget_ui import Ui_SMSWidget

from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging, sys

log = logging.getLogger("PyWengoPhone.SMSWidget")

class SMSWidget(QWidget):
    def __init__(self, tCoIpManager):
        QWidget.__init__(self, None)

        self.__tCoIpManager = tCoIpManager

        ## Init GUI
        self.__ui = Ui_SMSWidget()
        self.__ui.setupUi(self)
        ####

        connect(self.__ui.sendButton, QtCore.SIGNAL("clicked()"), self.sendButtonClicked)

    def sendButtonClicked(self):
        """
        Sends the SMS.
        """
        phoneNumbers = self.__ui.phoneNumbersLineEdit.text().toUtf8().data().split()
        if len(phoneNumbers) > 0:
            session = self.__tCoIpManager.getTSMSSessionManager().createTSMSSession()
            session.setPhoneNumberList(phoneNumbers)
            session.setMessage(self.__ui.textEdit.toPlainText().toUtf8().data())
            connect(session, SIGNAL("smsStateSignal(std::string, EnumSMSState::SMSState)"), self.smsStateSlot)
            self.__ui.statusLabel.setText(self.tr("Sending text..."))
            session.start()

    def smsStateSlot(self, number, state):
        if state == EnumSMSState.SMSStateOk:
            self.__ui.statusLabel.setText(self.tr("Text successfully sent to %1").arg(number))
        else:
            self.__ui.statusLabel.setText(self.tr("Error while sending text to %1").arg(number))
