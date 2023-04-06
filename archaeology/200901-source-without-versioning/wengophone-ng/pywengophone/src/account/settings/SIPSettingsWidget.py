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
# Test program for PyCoIpManager
#
# @author Ovidiu Ciule
# @author Philippe Bernery

from SIPAccountSettings_ui import Ui_SIPAccountSettings

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from PyCoIpManager import *

import logging, os, sys

log = logging.getLogger("PyWengoPhone.EditAccountDialog.SIPSettingsWidget")

class SIPSettingsWidget(QWidget):
    """
      SIP Settings Widget
      Each Account type (SIP, Wengo, GTalk etc.) has specific settings, 
      therefore they use specific GUI and code
    """

    def __init__(self, parent, account):
        # Init GUI     
        QWidget.__init__(self, parent)
        self._ui = Ui_SIPAccountSettings()
        self._ui.setupUi(self)

        sipaccount = SipAccount(account.getPrivateAccount())
        self._ui.realmLineEdit.setText(sipaccount.getRealm())
        self._ui.displaynameLineEdit.setText(sipaccount.getDisplayName())
        self._ui.registerServerLineEdit.setText(sipaccount.getRegisterServerHostname())
        # TODO fix after API update
        #self._ui.activateSIMPLE.setChecked({True: Qt.Checked, False: Qt.Unchecked}[sipaccount. ??isPIMEnabled - method does not exist anymore?? ()])
        self._ui.proxyServerLineEdit.setText(sipaccount.getSIPProxyServerHostname())
        self._ui.proxyPortLineEdit.setText(str(sipaccount.getSIPProxyServerPort()))
        # Here we convert a bool to Qt.CheckState
        forceProxy = {True: Qt.Checked, False: Qt.Unchecked}[sipaccount.useSIPProxyServer()]
        self._ui.forceProxy.setChecked(forceProxy)

    def modifyAccount(self, account):
        sipAccount = SipAccount(account.getPrivateAccount())
        sipAccount.setRealm(str(self._ui.realmLineEdit.text()))
        sipAccount.setDisplayName(str(self._ui.displaynameLineEdit.text()))
        sipAccount.setRegisterServerHostname(str(self._ui.registerServerLineEdit.text()))
        sipAccount.setSIPProxyServerHostname(str(self._ui.proxyServerLineEdit.text()))
        # Defensive programming: check if proxyPort really is an int
        try:
            proxyPort = int(self._ui.proxyPortLineEdit.text())
            sipAccount.setSIPProxyServerPort(proxyPort)
        except: pass
        # convert Qt.CheckedState to bool
        boolUseSipProxy = {Qt.Checked: True, Qt.Unchecked: False}[self._ui.forceProxy.checkState()]
        sipAccount.setUseSIPProxyServer(boolUseSipProxy)
        # TODO set SIMPLE support after API update
        return Account(sipAccount)
        

