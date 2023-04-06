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

from ContactWidget_ui import Ui_ContactWidget
from filetransfer import FileTransferWidget

from ContactEditWidget import ContactEditWidget
from call import *
from chat import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.ContactWidget")

class ContactWidget(QWidget):
    """ This class manages the contact widget.
    """
    def __init__(self, parent, contact, tCoIpManager):
        QWidget.__init__(self, parent)

        self.__contact = contact
        self.__tCoIpManager = tCoIpManager

        ## Initializing UI
        self.__ui = Ui_ContactWidget()
        self.__ui.setupUi(self)
        ####

        ## Registering to ContactWidget signals
        QObject.connect(self.__ui.avatarButton, SIGNAL("clicked()"), self.avatarButtonClicked)
        QObject.connect(self.__ui.callButton, SIGNAL("clicked()"), self.callButtonClicked)
        QObject.connect(self.__ui.chatButton, SIGNAL("clicked()"), self.chatButtonClicked)
        QObject.connect(self.__ui.sendFileButton, SIGNAL("clicked()"), self.fileButtonClicked)
        ####

        self.updateWidget()
    
    def setContact(self, contact):
        self.__contact = contact
        self.updateWidget()

    def updateWidget(self):
        ## Buttons
        if self.__tCoIpManager.getTCallSessionManager().canCreateTSession([self.__contact]):
            self.__ui.callButton.setEnabled(True)
        else:
            self.__ui.callButton.setEnabled(False)
    
        if self.__tCoIpManager.getTChatSessionManager().canCreateTSession([self.__contact]):
            self.__ui.chatButton.setEnabled(True)
        else:
            self.__ui.chatButton.setEnabled(False)

        if self.__tCoIpManager.getTFileSessionManager().canCreateTSession([self.__contact]):
            self.__ui.sendFileButton.setEnabled(True)
        else:
            self.__ui.sendFileButton.setEnabled(False)

        self.__ui.smsButton.setEnabled(False)
        ####

        ## Avatar
        backgroundPixmapFilename = ":/pics/avatar_background.png"
        foregroundPixmapData = self.__contact.getIcon().getData()
        pixmap = PixmapMerging.merge(foregroundPixmapData, backgroundPixmapFilename)
        self.__ui.avatarButton.setIcon(QIcon(pixmap))
        ####

    def avatarButtonClicked(self):
        editWidget = ContactEditWidget(self.__contact, self.__tCoIpManager, self)
        if editWidget.exec_() == QDialog.Accepted:
            self.__tCoIpManager.getTUserProfileManager().getTContactManager().update(self.__contact)

    def callButtonClicked(self):
        """ Call button was clicked. A call window will be opened.
        """
        CallManager.getInstance().callContact(self.__contact)

    def chatButtonClicked(self):
        """ Chat button was clicked, we will now open a chat window.
        """
        chatWidget = ChatMainWindow.getInstance()
        chatWidget.show()
        chatWidget.createChatTabFromContact(self.__tCoIpManager, self.__contact)

    def fileButtonClicked(self):
        """ Send file button was clicked. Open a send file widget.
        """
        fileName = QFileDialog.getOpenFileName(None, "Choose the file to send")
        FileTransferWidget.getInstance().sendFile(fileName, self.__contact)
