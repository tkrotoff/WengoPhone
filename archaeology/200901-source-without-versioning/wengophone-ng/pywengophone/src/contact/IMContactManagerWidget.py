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
# @author Philippe Bernery

from AddIMContactDialog import AddIMContactDialog
from IMContactManager_ui import Ui_IMContactManager

from PyCoIpManager import *

from PyQt4.Qt import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.IMContactManagerWidget")

class IMContactManagerWidget(QWidget):
    """ Widget for editing imcontacts of a contact.
    """
    def __init__(self, contact, tCoIpManager, parent=None):
        QWidget.__init__(self, parent)

        self.__contact = contact
        self.__tCoIpManager = tCoIpManager

        ## Initializing UI
        self.__ui = Ui_IMContactManager()
        self.__ui.setupUi(self)
    	####

        ## Initializing add button menu
        menu = QMenu(self)
        menu.addAction("AIM")
        menu.addAction("GTalk")
        menu.addAction("ICQ")
        menu.addAction("Jabber")
        menu.addAction("MSN")
        menu.addAction("SIP")
        menu.addAction("Wengo")
        menu.addAction("Yahoo")
        self.__ui.addIMContactButton.setMenu(menu)
        ####

        ## Initialize imcontact list content
        self.updateIMContactListContent()
        ####

        ## Signals
        QObject.connect(menu, SIGNAL("triggered(QAction *)"), self.addIMContactTriggered)
        QObject.connect(self.__ui.deleteIMContactButton, SIGNAL("clicked()"), self.removeIMContactClicked)
        ####

    def addIMContactTriggered(self, action):
        """ Called when addIMContact button is clicked.
        """
        accountType = EnumAccountType.toAccountType(str(action.text()))
        userProfile = self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile()
        accountList = AccountListHelper.getCopyOfAccountsOfProtocol(userProfile.getAccountList(), accountType)
        addIMContactDialog = AddIMContactDialog(accountType, accountList, self)
        if addIMContactDialog.exec_() == QDialog.Accepted:
            imContact = IMContact(accountType, addIMContactDialog.getContactId())
            imContact.setAccountId(addIMContactDialog.getSelectedAccountId())
            self.__contact.addIMContact(imContact)
            self.updateIMContactListContent()

    def removeIMContactClicked(self):
        """ Remove IMContact button has been clicked. Remove selected IMContact.
        """
        currentItem = self.__ui.treeWidget.currentItem()
        if currentItem != None:
            self.__contact.removeIMContact(str(currentItem.text(2)))
            self.updateIMContactListContent()

    def updateIMContactListContent(self):
        """ Update imcontact list content.
        """
        self.__ui.treeWidget.clear()
        imContactList = self.__contact.getIMContactList()
        for imContact in imContactList:
            treeWidgetItem = QTreeWidgetItem(self.__ui.treeWidget)
            treeWidgetItem.setText(0, imContact.getContactId())
            treeWidgetItem.setText(1, EnumAccountType.toString(imContact.getAccountType()))
            treeWidgetItem.setText(2, imContact.getUUID())
