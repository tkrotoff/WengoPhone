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

from JabberSettings_ui import Ui_JabberSettings

from util.QtUtil import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging, os, sys

log = logging.getLogger("PyWengoPhone.EditAccountDialog.JabberSettingsWidget")

class JabberSettingsWidget(QWidget):
    """
      Jabber Settings Widget
      Each Account type (SIP, Wengo, Jabber etc.) has specific settings, 
      therefore they use specific GUI and code
    """

    def __init__(self, parent, account):
        # Init GUI     
        QWidget.__init__(self, parent)
        self._ui = Ui_JabberSettings()
        self._ui.setupUi(self)

        # Settings form is filled in
        jabberaccount = JabberAccount(account.getPrivateAccount())
        self._ui.useTLSCheckBox.setChecked(boolToQtCheckState(jabberaccount.isTLSUsed()))
        self._ui.requireTLSCheckBox.setChecked(boolToQtCheckState(jabberaccount.isTLSRequired()))
        self._ui.forceOldSSLCheckBox.setChecked(boolToQtCheckState(jabberaccount.isOldSSLUsed()))
        self._ui.allowPlainTextAuthenticationCheckBox.setChecked(boolToQtCheckState(jabberaccount.isAuthPlainInClearUsed()))
        self._ui.connectServerLineEdit.setText(jabberaccount.getConnectionServer())
        self._ui.portLineEdit.setText(str(jabberaccount.getServerPort()))

    def modifyAccount(self, account):
        """
          Copies setting from the form to the account
        """
        jabberaccount = JabberAccount(account.getPrivateAccount())
        jabberaccount.setTLS(qtCheckStateToBool(self._ui.useTLSCheckBox.checkState()))
        jabberaccount.setTLSRequired(qtCheckStateToBool(self._ui.requireTLSCheckBox.checkState()))
        jabberaccount.setOldSSLUsed(qtCheckStateToBool(self._ui.forceOldSSLCheckBox.checkState()))
        jabberaccount.setAuthPlainInClearUsed(qtCheckStateToBool(self._ui.allowPlainTextAuthenticationCheckBox.checkState()))
        jabberaccount.setConnectionServer(str(self._ui.connectServerLineEdit.text()))
        try:
            port = int(self._ui.portLineEdit.text())
            jabberaccount.setServerPort(port)
        except: pass
        outAccount = Account(jabberaccount)
        return outAccount

