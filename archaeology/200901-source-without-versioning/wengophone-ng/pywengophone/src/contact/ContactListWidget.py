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

from ContactDelegate import ContactDelegate
from ContactEditWidget import ContactEditWidget
from ContactListModel import ContactListModel
from ContactListWidget_ui import Ui_ContactListWidget

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.ContactListWidget")

class ContactListWidget(QWidget):
    """ This class manages the contact list widget.
    """
    def __init__(self, parent, tCoIpManager):
        QWidget.__init__(self, parent)

        self.__tCoIpManager = tCoIpManager
        self.__lastOpenedIndex = QModelIndex()

        ## Initializing UI
        self.__ui = Ui_ContactListWidget()
        self.__ui.setupUi(self)
        ####

        ## Registering to ContactListWidget signals
        QObject.connect(self.__ui.removeContactButton, SIGNAL("clicked()"), self.removeContactClicked)
        QObject.connect(self.__ui.contactListView, SIGNAL("clicked(const QModelIndex &)"), self.contactListViewClicked)
        ####

        ## Setting model
        contactDelegate = ContactDelegate(self.__tCoIpManager, self)
        self.__ui.contactListView.setItemDelegate(contactDelegate)
        contactListModel = ContactListModel(self.__tCoIpManager, self)
        self.__ui.contactListView.setModel(contactListModel)
        ####

    def contactListViewClicked(self, index):
        if self.__lastOpenedIndex.isValid():
            self.__ui.contactListView.closePersistentEditor(self.__lastOpenedIndex)
            self.__ui.contactListView.itemDelegate().resetEditedIndex()

        if self.__lastOpenedIndex != index:
            self.__ui.contactListView.openPersistentEditor(index)
            self.__lastOpenedIndex = index
        else:
            self.__lastOpenedIndex = QModelIndex()

        self.__ui.contactListView.doItemsLayout()

    def addContactClicked(self):
        contact = Contact()
        editWidget = ContactEditWidget(contact, self.__tCoIpManager, self)
        if editWidget.exec_() == QDialog.Accepted:
            self.__tCoIpManager.getTUserProfileManager().getTContactManager().add(contact)

    def removeContactClicked(self):
        indexList = self.__ui.contactListView.selectedIndexes()
        if len(indexList) > 0:
            index = indexList[0]
            contact = index.model().getContact(index)
            if QMessageBox.question(self, self.tr("Remove a contact"),
                self.tr("Are you sure you want to remove %1?").arg(contact.getDisplayName()), QMessageBox.Yes, QMessageBox.No) == QMessageBox.Yes:
                self.__tCoIpManager.getTUserProfileManager().getTContactManager().remove(contact.getUUID())
