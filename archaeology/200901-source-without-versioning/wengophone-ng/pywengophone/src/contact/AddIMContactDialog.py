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

from AddIMContactWidget_ui import Ui_AddIMContactWidget

from PyCoIpManager import *

from PyQt4.Qt import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.AddIMContactDialog")

class AddIMContactDialog(QDialog):
    """ Widget for adding IMContact.
    """
    def __init__(self, accountType, accountList, parent=None):
        QDialog.__init__(self, parent)

        ## Initializing UI
        self.__ui = Ui_AddIMContactWidget()
        self.__ui.setupUi(self)
        self.__ui.addButton.setEnabled(False)
    	####

        ## Initializing content of accountComboBox
        for account in accountList:
            self.__ui.accountComboBox.addItem(account.getLogin(), QVariant(account.getUUID()))
        ####

        #TODO: display accountType on the dialog

        ## Signals
        QObject.connect(self.__ui.addButton, SIGNAL("clicked()"), self.addButtonClicked)
        QObject.connect(self.__ui.cancelButton, SIGNAL("clicked()"), self.cancelButtonClicked)
        QObject.connect(self.__ui.contactIdLineEdit, SIGNAL("textEdited(const QString &)"), self.contactIdEdited)
        ####

    def addButtonClicked(self):
        self.accept()

    def cancelButtonClicked(self):
        self.reject()

    def contactIdEdited(self, text):
        if len(text) > 0:
            self.__ui.addButton.setEnabled(True)
        else:
            self.__ui.addButton.setEnabled(False)

    def getContactId(self):
        """ Returns the entered contact id.
        """
        return str(self.__ui.contactIdLineEdit.text())

    def getSelectedAccountId(self):
        """ Returns the selected account id.
        """
        return str(self.__ui.accountComboBox.itemData(self.__ui.accountComboBox.currentIndex()).toString())
