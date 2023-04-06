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

from EditAccountDialog_ui import Ui_EditAccountDialog
from account.settings import *

from util.QtUtil import *
from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging, os, sys

log = logging.getLogger("PyWengoPhone.EditAccountDialog")

class EditAccountDialog(QDialog):
    """
      EditAccountDialog: dialog for editing an Account
      Each Account type (SIP, Wengo, GTalk etc.) has specific settings, 
      therefore they use specific GUI and code, in 
      ($AccountType)SettingsWidget.py
    """

    def __init__(self, parent, account):
        # Init GUI
        QDialog.__init__(self, parent)
        self._ui = Ui_EditAccountDialog()
        self._ui.setupUi(self)

        self.__account = account
        self.__settingsWidget = self.getSettingsWidget(account)
        self.__settingsWidget.layout().setMargin(0)

        # Here we set up the Network Settings tab; we add a network-specific
        # settings widget
        qvblayout = QVBoxLayout(self._ui.settingsWidget)
        self._ui.settingsWidget.layout().addWidget(self.__settingsWidget)
        self.setWindowTitle(self.__settingsWidget.windowTitle())
        
        if account.getConnectionState()==EnumConnectionState.ConnectionStateDisconnected:
            #if disconnected, we can edit
            pass
        else:
            self.setEnableEditing(False)

        # Fill in form with account data
        self._ui.loginLineEdit.setText(account.getLogin())
        self._ui.passwordLineEdit.setText(account.getPassword())
        self._ui.savePasswordCheckBox.setCheckState(boolToQtCheckState(account.isPasswordSaved()))
        # set up presenceStateComboBox: fill it with presence states
        presStates = [ EnumPresenceState.PresenceStateAway, \
                EnumPresenceState.PresenceStateDoNotDisturb, \
		EnumPresenceState.PresenceStateInvisible, \
		EnumPresenceState.PresenceStateOffline, \
		EnumPresenceState.PresenceStateOnline]
        presStatesStr = map(EnumPresenceState.toString, \
             presStates)
        #presStatesStr = map(str.title, presStatesStr)
        self._ui.presenceStateComboBox.addItems(presStatesStr)
        # Get current presence state
        pres = account.getPresenceState()
        # We can't show 'unknown' or 'unavailable', so we map those to online
        if pres == EnumPresenceState.PresenceStateUnknown or \
            pres == EnumPresenceState.PresenceStateUnavailable:
            pres = EnumPresenceState.PresenceStateOnline
        # ... And show it
        self._ui.presenceStateComboBox.setCurrentIndex(presStates.index(pres))
        # Alias/displayname/status
        self._ui.statusLineEdit.setText(account.getAlias())

        connect(self._ui.saveButton, SIGNAL("clicked()"), self.accept)
        connect(self._ui.cancelButton, SIGNAL("clicked()"), self.reject)
        self._ui.tabWidget.setCurrentIndex(0)
        self.enableNetworkSettingsTab()

    def enableNetworkSettingsTab(self):
        """
        Enables/disables the NetworkSettings tab according to needs
        Wengo and GTalk accounts do not need it, as they have no network
        settings
        """
        # TODO TO FIX
        if len(self.__settingsWidget.children())<1:
            enabled = False
        else:
            enabled = True
        self._ui.tabWidget.setTabEnabled(2, enabled)

    def getAccount(self):
        """ 
        Returns the modified (or not) account
        """
        # modify account data: login, password, savePassword, presenceState
        # TODO: check if account was modified
        self.__account.setLogin(str(self._ui.loginLineEdit.text()))
        self.__account.setPassword(str(self._ui.passwordLineEdit.text()))
        self.__account.setSavePassword(qtCheckStateToBool(self._ui.savePasswordCheckBox.checkState()))
        # Presence State
        pres = EnumPresenceState.fromString(str(self._ui.presenceStateComboBox.currentText()))
        if pres!=None and \
            pres != EnumPresenceState.PresenceStateUnavailable and \
            pres != EnumPresenceState.PresenceStateUnknown:
            self.__account.setPresenceState(pres)
        # Alias/status
        self.__account.setAlias(str(self._ui.statusLineEdit.text()))
        return self.__settingsWidget.modifyAccount(self.__account)

    def getSettingsWidget(self, account):
        """
          Factory method that creates network-specific settings widget
        """
        try:
            accountSettingsWidget = {EnumAccountType.AccountTypeSIP: lambda(account): SIPSettingsWidget(self, account), \
                EnumAccountType.AccountTypeWengo: lambda(account): WengoSettingsWidget(self, account), \
                EnumAccountType.AccountTypeGTalk: lambda(account): GTalkSettingsWidget(self, account), \
                EnumAccountType.AccountTypeYahoo: lambda(account): YahooSettingsWidget(self, account), \
                EnumAccountType.AccountTypeJabber: lambda(account): JabberSettingsWidget(self, account), \
                EnumAccountType.AccountTypeMSN: lambda(account): MSNSettingsWidget(self, account), \
                EnumAccountType.AccountTypeUnknown: lambda(account): QWidget()}[account.getAccountType()](account)
        except KeyError:
            accountSettingsWidget = QWidget()
        return accountSettingsWidget

    def setEnableEditing(self, enabled):
        log.info("setEnableEditing("+str(enabled)+")")
        self._ui.saveButton.setEnabled(enabled)
        for child in self.__settingsWidget.children():
            if isinstance(child, QWidget):
              child.setEnabled(enabled)

