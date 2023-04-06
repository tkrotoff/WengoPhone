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

from ContactListModel import ContactListModel
from ContactWidget import ContactWidget

from PyQt4.Qt import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.ContactDelegate")

class ContactDelegate(QItemDelegate):
    """ Delegate to display and edit contacts.
    """
    def __init__(self, tCoIpManager, parent=None):
        QItemDelegate.__init__(self, parent)

        self.__editedIndex = QModelIndex()
        self.__tCoIpManager = tCoIpManager
        
    def paint(self, painter, option, index):
        QItemDelegate.paint(self, painter, option, index)

    def sizeHint(self, option, index):
        size = QItemDelegate.sizeHint(self, option, index)
        if index == self.__editedIndex:
            size.setHeight(95) # Size taken from ContactWidget.ui
        return size

    def resetEditedIndex(self):
        """ Called when a persistent editor is closed.
        """
        self.__editedIndex = QModelIndex()

    def createEditor(self, parent, option, index):
        return ContactWidget(parent, index.model().getContact(index), self.__tCoIpManager)

    def setEditorData(self, editor, index):
        self.__editedIndex = index
        editor.setContact(index.model().getContact(index))
