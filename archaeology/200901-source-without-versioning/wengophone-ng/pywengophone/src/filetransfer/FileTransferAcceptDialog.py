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
from AcceptDialog_ui import Ui_AcceptDialog

from PyQt4.QtGui import *

class FileTransferAcceptDialog(QDialog):
    def __init__(self, parent=None):
        QDialog.__init__(self, parent)

        ## Setting up ui
        self.__ui = Ui_AcceptDialog()
        self.__ui.setupUi(self)
        ####

    def setFileName(self, fileName):
        self.__ui.filenameLabel.setText(fileName)

    def setContactName(self, contactName):
        self.__ui.contactLabel.setText(contactName)
