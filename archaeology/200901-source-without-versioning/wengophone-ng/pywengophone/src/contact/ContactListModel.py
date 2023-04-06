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

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging
import sys

log = logging.getLogger("PyWengoPhone.ContactListModel")

class ContactListModel(QAbstractListModel):
    """ ContactList implementation of QAbstractListModel.
    """
    ContactRole = QVariant.UserType + 1

    def __init__(self, tCoIpManager, parent=None):
        QAbstractListModel.__init__(self, parent)

        self.__tCoIpManager = tCoIpManager

        ## Initializing presence icon
        self.__presenceIconDict = dict()
        self.__presenceIconDict[EnumPresenceState.PresenceStateOnline] = QPixmap(":/pics/status/online.png")
        self.__presenceIconDict[EnumPresenceState.PresenceStateOffline] = QPixmap(":/pics/status/offline.png")
        self.__presenceIconDict[EnumPresenceState.PresenceStateInvisible] = QPixmap(":/pics/status/invisible.png")
        self.__presenceIconDict[EnumPresenceState.PresenceStateAway] = QPixmap(":/pics/status/away.png")
        self.__presenceIconDict[EnumPresenceState.PresenceStateDoNotDisturb] = QPixmap(":/pics/status/donotdisturb.png")
        self.__presenceIconDict[EnumPresenceState.PresenceStateUnknown] = QPixmap(":/pics/status/unknown.png")
        self.__presenceIconDict[EnumPresenceState.PresenceStateUnavailable] = QPixmap(":/pics/status/unknown.png")
        ####

        ## Registering to TCoIpManager signals
        QObject.connect(self.__tCoIpManager.getTUserProfileManager().getTContactManager(),
            SIGNAL("contactAddedSignal(std::string)"),
            self.contactAddedSlot)
        QObject.connect(self.__tCoIpManager.getTUserProfileManager().getTContactManager(),
            SIGNAL("contactRemovedSignal(std::string)"),
            self.contactRemovedSlot)
        QObject.connect(self.__tCoIpManager.getTUserProfileManager().getTContactManager(),
            SIGNAL("contactUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)"),
            self.contactUpdatedSlot)
        ####

        ## Initializes data in model
        self.__initialize()
        ####

    def __initialize(self):
        """
        Synchonizes data with CoIpManager.
        """
        contactList = self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getContactList()
        self.__contactDict = dict()
        for contact in contactList:
            self.__contactDict[contact.getUUID()] = contact

        self.emit(SIGNAL("layoutChanged()"))
 
    def rowCount(self, parent):
        return len(self.__contactDict)

    def data(self, index, role):
        result = QVariant()

        if not index.isValid():
            return QVariant()

        if index.row() > (len(self.__contactDict) - 1):
            return QVariant()

        contactList = self.__contactDict.items()
        contact = contactList[index.row()][1]
        if role == Qt.DisplayRole:
            if len(contact.getAlias()) > 0:
                result = QVariant(QString.fromUtf8(contact.getAlias()))
            else:
                result = QVariant(QString.fromUtf8(contact.getDisplayName()))
        elif role == Qt.ToolTipRole:
            result = QVariant(QString.fromUtf8(contact.getDisplayName()))
        elif role == Qt.DecorationRole:
            result = QVariant(self.__presenceIconDict[contact.getPresenceState()])
        elif role == ContactListModel.ContactRole:
            result = contact

        return result

    def getContact(self, index):
        return self.data(index, ContactListModel.ContactRole)

    def flags(self, index):
        return Qt.ItemIsEnabled | Qt.ItemIsSelectable | Qt.ItemIsEditable

    def contactAddedSlot(self, contactId):
        contactList = self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getContactList()
        self.__contactDict[contactId] = ContactListHelper.getCopyOfContact(contactList, contactId)

        self.emit(SIGNAL("layoutChanged()"))

    def contactRemovedSlot(self, contactId):
        del self.__contactDict[contactId]

        self.emit(SIGNAL("layoutChanged()"))

    def contactUpdatedSlot(self, contactId, section):
        contactList = self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getContactList()
        self.__contactDict[contactId] = ContactListHelper.getCopyOfContact(contactList, contactId)

        self.emit(SIGNAL("layoutChanged()"))
