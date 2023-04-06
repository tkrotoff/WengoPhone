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

from AccountListManagerWidget_ui import Ui_AccountListManagerWidget
from EditAccountDialog import EditAccountDialog
from NewAccountDialog import NewAccountDialog
from config import Config

from util.QtUtil import *
from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging, os, sys

log = logging.getLogger("PyWengoPhone.AccountListManagerWidget")

class AccountListManagerWidget(QDialog):
    def __init__(self, parent, tCoIpManager):
        """
        Pre-condition: tCoIpManager must be initialized, otherwise Config, UserProfile would not be useful
        """
        # Init GUI     
        QDialog.__init__(self, parent)
        self._ui = Ui_AccountListManagerWidget()
        self._ui.setupUi(self)

        connect(self._ui.closeButton, SIGNAL("clicked()"), self.close)
        self.__tCoIpManager = tCoIpManager

        connect(self._ui.addButton, SIGNAL("clicked()"), self.addButtonClicked)
        connect(self._ui.modifyButton, SIGNAL("clicked()"), self.modifyButtonClicked)
        connect(self._ui.deleteButton, SIGNAL("clicked()"), self.deleteButtonClicked)
        connect(self._ui.treeWidget, SIGNAL(" itemActivated (QTreeWidgetItem *,int)"), self.itemActivated)
        connect(self._ui.treeWidget, SIGNAL(" itemChanged (QTreeWidgetItem *,int)"), self.itemChangedSlot)
        
        self.loadAccountList()
 
    def addButtonClicked(self):
        """
        Adds a new account
        """
        # Show NewAccountDialog
        nad = NewAccountDialog(self)
        # If Yes
        if nad.exec_() == QDialog.Accepted:
            account = nad.getAccount()
            # Add the account
            self.__tCoIpManager.getTUserProfileManager().getTAccountManager().add(account)
            # check if account really was added; otherwise -> crash when trying to connect it
            # Account will not be added if duplicate, or if it has incorrect data
            accList = self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getAccountList()
            if AccountListHelper.getCopyOfAccount(accList, account.getLogin(), account.getAccountType())!=None:
                # And connect it
                self.__tCoIpManager.getTConnectManager().connect(account.getUUID())
        self.loadAccountList()

    def loadAccountList(self):
        """
        Refreshes the account list
        """
        self._ui.treeWidget.clear()
        for account in self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getAccountList():
            sl = QStringList("")
            sl.append(EnumAccountType.toString(account.getAccountType()))
            sl.append(account.getLogin())
            sl.append(account.getUUID())
            qTWI = QTreeWidgetItem(sl)
            qTWI.setFlags(qTWI.flags() | Qt.ItemIsUserCheckable)
            # Is the account enabled? Should we check the Enable checkBox?
            enabled = (account.getConnectionState()!=EnumConnectionState.ConnectionStateDisconnected)
            qTWI.setCheckState(0, boolToQtCheckState(enabled))
            self._ui.treeWidget.addTopLevelItem(qTWI)

    def deleteButtonClicked(self):
        """
        Deletes the currently selected account
        """
        account = self.getSelectedAccount()
        if account == None:
            return
        self.__tCoIpManager.getTConnectManager().disconnect(account.getUUID())
        self.__tCoIpManager.getTUserProfileManager().getTAccountManager().remove(account.getUUID())
        log.info("Deleted: "+account.getUUID())
        self.loadAccountList()

    def modifyButtonClicked(self):
        """
        Edits an account
        """
        account = self.getSelectedAccount()
        if account == None:
            return
        # Show EditAccountDialog
        ead = EditAccountDialog(self, account)
        # If Yes
        if ead.exec_() == QDialog.Accepted:
            outAccount = ead.getAccount()
            # TODO: update can be given EnumUpdateSection, which optimizes the update
            self.__tCoIpManager.getTUserProfileManager().getTAccountManager().update(outAccount)
            #self.__tCoIpManager.getTConnectManager().disconnect(outAccount.getUUID())
            #self.__tCoIpManager.getTConnectManager().connect(outAccount.getUUID())
        self.loadAccountList()

    def getSelectedAccount(self):
        """
        Returns the account corresponding to the currently selected item

        Pre-condition:
        Account items must be in the same order as the AccountList returned by UserProfile.getAccountList()
        """
        # TODO fix it so that it works regardless of the order
        accountItem = self._ui.treeWidget.currentItem()
        if accountItem == None:
             return None
        ixAccount = self._ui.treeWidget.indexOfTopLevelItem(accountItem)
        accounts = self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getAccountList()
        return accounts[ixAccount]

    def itemActivated(self, item, col):
        """
        Opens the double-clicked account for editing
        """
        self.modifyButtonClicked()

    def itemChangedSlot(self, item, col):
        """
        Listens to enableCheckBox changes, and connects/disconnects the acount
        """
        if col==0:
            enabled = qtCheckStateToBool(item.checkState(0))
            account = self.getSelectedAccount()
            if account==None: return
            if enabled:
                log.debug("itemChanged: Account: "+str(account.getLogin())+str(account.getUUID())+"/" \
                    +str(EnumAccountType.toString(account.getAccountType())) \
                    +" goes online")
                self.__tCoIpManager.getTConnectManager().connect(account.getUUID())
            else:
                log.debug("itemChanged: Account: "+str(account.getLogin())+str(account.getUUID())+"/" \
                    +str(EnumAccountType.toString(account.getAccountType())) \
                    +" goes offline")
                self.__tCoIpManager.getTConnectManager().disconnect(account.getUUID())
            
