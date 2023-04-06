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

from config import Config
from FileTransferItem_ui import Ui_FileTransferItem
from util.SafeConnect import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

class FileTransferItem(QWidget):
    def __init__(self, session, parent=None):
        QWidget.__init__(self, parent)

        self._session = session

        ## Setting up ui
        self.__ui = Ui_FileTransferItem()
        self.__ui.setupUi(self)
        ####

        self.__ui.progressBar.setMaximum(100)
        self.setProgress(0)

        connect(session,
            SIGNAL("fileTransferSignal(IFileSession::IFileSessionEvent)"),
            self.updateState)
        connect(session,
            SIGNAL("fileTransferProgressionSignal(int)"),
            self.setProgress)

    def setFilename(self, filename):
        self.__ui.filenameLabel.setText("<b>" + filename + "</b>")

    def setState(self, state):
        self.__ui.statusLabel.setText(state)

    def setContact(self, contact):
        self.__contact = contact
        
    def setContactLabel(self, text):
        self.__ui.contactLabel.setText(text)

    def setProgress(self, progress):
        self.__ui.progressBar.setValue(progress)
        #HACK: IFileSessionEventFileTransferFinished is not always sent
        if progress != 100:
            self.stateChangeEventDownUp()
        else:
            self.updateButtonsFinished()

    def updateButtonsFinished(self):
        self.disconnectButtons()
        connect(self.__ui.cancelOpenButton, SIGNAL("clicked()"), self.open)
        connect(self.__ui.removePauseResumeButton, SIGNAL("clicked()"), self.remove)
        self.__ui.cancelOpenButton.setText(self.tr("Open"))
        self.__ui.cancelOpenButton.setEnabled(True)
        self.__ui.removePauseResumeButton.setText(self.tr("Remove"))
        self.__ui.removePauseResumeButton.setEnabled(True)
        #FIXME: remove the status bar from its layout
        #self.__ui.progressBar.hide();

    def updateButtonsPaused(self):
        self.disconnectButtons()
        connect(self.__ui.cancelOpenButton, SIGNAL("clicked()"), self._session.stop)
        connect(selg.__ui.removePauseResumeButton, SIGNAL("clicked()"), session.resume)
        self.__ui.cancelOpenButton.setText(self.tr("Cancel"))
        self.__ui.cancelOpenButton.setEnabled(False)
        self.__ui.removePauseResumeButton.setText(self.tr("Resume"))
        self.__ui.removePauseResumeButton.setEnabled(True)

    def updateButtonsPausedByPeer(self):
        self.disconnectButtons()
        connect(self.__ui.cancelOpenButton, SIGNAL("clicked()"), self._session.stop)
        connect(self.__ui.removePauseResumeButton, SIGNAL("clicked()"), self._session.pause)
        self.__ui.cancelOpenButton.setText(self.tr("Cancel"))
        self.__ui.cancelOpenButton.setEnabled(False)
        self.__ui.removePauseResumeButton.setText(self.tr("Pause"))
        self.__ui.removePauseResumeButton.setEnabled(False)

    def updateButtonsInProgress(self):
        self.disconnectButtons()
        connect(self.__ui.cancelOpenButton, SIGNAL("clicked()"), self._session.stop)
        connect(self.__ui.removePauseResumeButton, SIGNAL("clicked()"), self._session.pause)
        self.__ui.cancelOpenButton.setText(self.tr("Cancel"))
        self.__ui.cancelOpenButton.setEnabled(True)
        self.__ui.removePauseResumeButton.setText(self.tr("Pause"))
        self.__ui.removePauseResumeButton.setEnabled(True)

    def disconnectButtons(self):
        QObject.disconnect(self.__ui.cancelOpenButton, SIGNAL("clicked()"), self._session.stop)
        QObject.disconnect(self.__ui.removePauseResumeButton, SIGNAL("clicked()"), self._session.resume)
        QObject.disconnect(self.__ui.removePauseResumeButton, SIGNAL("clicked()"), self._session.pause)
        QObject.disconnect(self.__ui.removePauseResumeButton, SIGNAL("clicked()"), self.remove)

    def remove(self):
        self.emit(SIGNAL("removeClicked"))

    def open(self):
        pass

    def updateState(event):
        if event == IFileSession.IFileSessionEventInviteToTransfer:
            self.stateChangeEvent(self.tr("Starting"))
            self.updateButtonsInProgress()
        elif event == IFileSession.IFileSessionEventWaitingForAnswer:
            self.stateChangeEvent(self.tr("Waiting for anwser..."))
            self.updateButtonsInProgress()
        elif event == IFileSession.IFileSessionEventFileTransferFinished:
            self.stateChangeEvent(self.tr("Done"))
            self.updateButtonsFinished()
        elif event == IFileSession.IFileSessionEventFileTransferFailed:
            """
            self.stateChangeEvent(self.tr("Failed"))
            self.updateButtonsFinished()
            qtFileTransferNotifyDialog = QtFileTransferNotifyDialog(self)
            qtFileTransferNotifyDialog.setTitle(self.tr("An error occured during the file transfer"))
            qtFileTransferNotifyDialog.setMessage(tr("This may be caused by :") + "<br>" +
                "<ul>" +
                "<li>" + self.tr("Your @product@ is not up to date. Please download the latest version on") +
                " " + Config.getInstance().companyWebSiteUrl + "<br>" +
                "<li>" + self.tr("The @company@ network may be temporarily unavailable. Please try later.") +
                "</ul>");
            qtFileTransferNotifyDialog.exec_()
            """
            pass
        elif event == IFileSession.IFileSessionEventFileTransferPaused:
            self.setState(self.tr("Paused"))
            self.updateButtonsPaused()
        elif event == IFileSession.IFileSessionEventFileTransferPausedByPeer:
            self.setState(self.tr("Paused by peer"))
            self.updateButtonsPausedByPeer()
        elif event == IFileSession.IFileSessionEventFileTransferResumed:
            self.stateChangeEventDownUp()
            self.updateButtonsInProgress();
        elif event == IFileSession.IFileSessionEventFileTransferResumedByPeer:
            self.stateChangeEventDownUp()
            self.updateButtonsInProgress()
        elif event == IFileSession.IFileSessionEventFileTransferCancelled:
            self.setState(self.tr("Cancelled"))
            self.updateButtonsFinished()
        elif event == IFileSession.IFileSessionEventFileTransferCancelledByPeer:
            self.setState(self.tr("Cancelled by peer"))
            self.updateButtonsFinished()
        elif event == IFileSession.IFileSessionEventFileTransferBegan:
            self.stateChangeEventDownUp()
            self.updateButtonsInProgress()
        else:
            log.fatal("unknonw IFileSessionEvent: ", event)

    def stateChangeEventDownUp(self):
        pass
