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

from config import Config
from FileTransferAcceptDialog import FileTransferAcceptDialog
from FileTransferDialog_ui import Ui_FileTransferDialog
from FileTransferItemDownload import FileTransferItemDownload
from FileTransferItemUpload import FileTransferItemUpload
from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import sys, os
import logging

log = logging.getLogger("PyWengoPhone.FileTransferWidget")

class FileTransferWidget(QDialog):
    """ Widget displaying current file downloads/uploads.
    """
    __currentInstance = None
    def createInstance(tCoIpManager):
        if FileTransferWidget.__currentInstance == None:
            FileTransferWidget.__currentInstance = FileTransferWidget(tCoIpManager)
        else:
            log.info("instance already created. Use getInstance to get instance.")
    createInstance = staticmethod(createInstance)

    def getInstance():
        if FileTransferWidget.__currentInstance != None:
            return FileTransferWidget.__currentInstance
        else:
            log.fatal("instance not created. Call createInstance before calling getInstance.")
    getInstance = staticmethod(getInstance)

    def __init__(self, tCoIpManager, parent=None):
        QDialog.__init__(self, parent)

        self.__tCoIpManager = tCoIpManager

        ## Setting up ui
        self.__ui = Ui_FileTransferDialog()
        self.__ui.setupUi(self)
        ####

        ## Registering to TFileSessionManager events.
        connect(self.__tCoIpManager.getTFileSessionManager(),
            SIGNAL("newTReceiveFileSessionCreatedSignal(TReceiveFileSession *)"),
            self.newTReceiveFileSessionCreatedSlot)
        connect(self.__tCoIpManager.getTFileSessionManager(),
            SIGNAL("needsUpgradeSignal()"),
            self.needsUpgradeSlot)
        connect(self.__tCoIpManager.getTFileSessionManager(),
            SIGNAL("peerNeedsUpgradeSignal(Contact)"),
            self.peerNeedsUpgradeSlot)
        ####

    def sendFile(self, filename, contact):
        """ Send a file to a contact.
        """
        if len(filename) == 0:
            return

        fileToSend = File(str(filename.toUtf8()), File.EncodingUTF8)
        sendSession = self.__tCoIpManager.getTFileSessionManager().createTSendFileSession()
        sendSession.addContact(contact)
        sendSession.setFile(fileToSend)
        self.__addSendFileSession(sendSession)

    def __addSendFileSession(self, session):
        """ Add the session in the FileTransferWidget and starts it.
        """
        fileTransferItem = FileTransferItemUpload(session, self)
        connect(fileTransferItem, SIGNAL("removeClicked"), self.itemRemoveClicked)
        item = QListWidgetItem()
        item.setSizeHint(fileTransferItem.minimumSizeHint())
        self.__ui.uploadTransferListWidget.insertItem(0, item)
        self.__ui.uploadTransferListWidget.setItemWidget(item, fileTransferItem)
        self.__showUploadTab();
        self.__showAndRaise();

        session.start()

    def __addReceiveFileSession(self, session):
        fileTransferItem = FileTransferItemDownload(session, self)
        connect(fileTransferItem, SIGNAL("removeClicked"), self.itemRemoveClicked)
        item = QListWidgetItem()
        item.setSizeHint(fileTransferItem.minimumSizeHint())
        self.__ui.uploadTransferListWidget.insertItem(0, item)
        self.__ui.uploadTransferListWidget.setItemWidget(item, fileTransferItem)
        self.__showDownloadTab();
        self.__showAndRaise();

        session.start()

    def newTReceiveFileSessionCreatedSlot(self, receiveFileSession):
        acceptDialog = FileTransferAcceptDialog(self)
        acceptDialog.setFileName(receiveFileSession.getFileName())
        acceptDialog.setContactName(receiveFileSession.getContact().getDisplayName())
        if (acceptDialog.exec_() == QDialog.Accepted):
            receiveFileSession.setFilePath(Config.getInstance().fileTransferDownloadFolder)
            self.__addReceiveFileSession(receiveFileSession)

    def needsUpgradeSlot(self):
        log.debug("needsUpgradeSlot")

    def peerNeedsUpgradeSlot(self, contact):
        log.debug("peerNeedsUpgradeSlot")

    def __showDownloadTab(self):
        self.__ui.tabWidget.setCurrentIndex(0);
        self.__ui.downloadTransferListWidget.scrollToTop()
    
    def __showUploadTab(self):
        self.__ui.tabWidget.setCurrentIndex(1)
        self.__ui.uploadTransferListWidget.scrollToTop()

    def __showAndRaise(self):
        self.activateWindow()
        self.showNormal()

    def itemRemoveClicked(self):
        self.clean(false)