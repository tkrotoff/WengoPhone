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
# @author Aurelien Gateau
# @author Ovidiu Ciule

from ChatMainWindow_ui import Ui_ChatMainWindow
from ChatTabWidget import ChatTabWidget

from call import CallManager
from filetransfer import FileTransferWidget

from util.SafeConnect import *
from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import os, logging

log = logging.getLogger("PyWengoPhone.ChatMainWindow")

class ChatMainWindow(QMainWindow, Ui_ChatMainWindow):
    """
    Main chat window. Is a QMainWindow, because it needs a menu
    Each chat is a tab
    Implements Singleton pattern via getInstance()
    Cannot inherit util.Singleton because of SIP's weak support for multiple
    inheritance
    """
    def getInstance():
        if ChatMainWindow.__currentInstance==None:
            ChatMainWindow.__currentInstance = ChatMainWindow()
        return ChatMainWindow.__currentInstance
    getInstance = staticmethod(getInstance)

    """
    Variable holding the unique instance
    """
    __currentInstance = None

    def __init__(self):
        if ChatMainWindow.__currentInstance != None:
            # Should not get here, constructor should not be called directly
            raise RuntimeError, "Constructor has been called twice. Probably \
                it has been called directly.\n This is a Singleton. Please \
                use getInstance()"
            return

        QMainWindow.__init__(self)

        self.setupUi(self)
        self.createWidgets()

    def createWidgets(self):
        self.tabWidget = QTabWidget(self)
        self.setCentralWidget(self.tabWidget)

       # if !defined(OS_MACOSX)
        import platform
        if platform.system() != "Darwin":
            # If Win or Linux
            closeTabButton = QToolButton(self.tabWidget)
            closeIcon = QIcon(QPixmap(":/pics/close_normal.png"))
            closeIcon.addPixmap(QPixmap(":/pics/close_on.png"), QIcon.Normal, QIcon.On)
            closeTabButton.setIcon(closeIcon)
            closeTabButton.setToolTip("Close chat")
            closeTabButton.setAutoRaise(True)
            self.tabWidget.setCornerWidget(closeTabButton, Qt.TopRightCorner)
            connect(closeTabButton, SIGNAL("clicked()"), self.closeButtonClickedSlot)
        else:
            # On MacOS X, we should use a big close button in the toolbar
            # The corner close button is not MacOS-like, and against Apple GUI design
            # guidelines
            # Create the buttton
            closeTabButton = QToolButton(self.toolBar)
            closeIcon = QIcon(QPixmap(":/pics/chat/chat_close"))
            closeTabButton.setIcon(closeIcon)
            closeTabButton.setToolTip("Close chat")
            closeTabButton.setAutoRaise(True)
            closeTabButton.setFixedSize(44, 44)
            # Create a widget that will hold a horizontal spacer and the close button
            holder = QWidget(self.toolBar)
            layout = QHBoxLayout(holder)
            # insert a Spacer
            spacer = QSpacerItem(10, 0, QSizePolicy.Expanding)
            holder.layout().addItem(spacer)
            holder.layout().addWidget(closeTabButton)
            self.toolBar.addWidget(holder)
            connect(closeTabButton, SIGNAL("clicked()"), self.closeButtonClickedSlot)
 
        # Init status bar: calling statusbar() forces its drawing
        self.statusBar()
        # Disable all toolBar buttons
        actions = [self.actionSendSms, self.actionSendFile, \
            self.actionContactInfo, self.actionCreateChatConf]
        for action in actions: action.setEnabled(False)

    def initSignals(self, tCoIpManager):
        """ Called by MainWindow; Set ups signals
        """
        connect(tCoIpManager.getTChatSessionManager(), \
            SIGNAL("newChatSessionCreatedSignal (TChatSession *)"), 
            self.newChatSessionCreatedSlot)
        connect(self.tabWidget, \
            SIGNAL("currentChanged ( int )"), \
            self.tabChangedSlot)
        connect(self.actionCallContact, \
            SIGNAL("triggered (bool)"), \
            self.callContactSlot)
        connect(self.actionSendFile, \
            SIGNAL("triggered (bool)"), \
            self.sendFileSlot)
        connect(self.actionClose, \
            SIGNAL("triggered (bool)"), \
            self.closeActionTriggeredSlot)

        self.__tCoIpManager = tCoIpManager

    def tabChangedSlot(self, ixTab):
        """ Slot called when the current tab selection is changed
            We change the title of the window
        """
        tab = self.tabWidget.currentWidget()
        self.updateActionsAndTitles(tab)

    def updateActionsAndTitles(self, tab):
        # Enable/Disable Call action
        canCall = self.__tCoIpManager.getTCallSessionManager().canCreateTSession(tab.getContacts())
        self.actionCallContact.setEnabled(canCall)

        # Enable/Disable Call action
        canSendFile = self.__tCoIpManager.getTFileSessionManager().canCreateTSession(tab.getContacts())
        self.actionSendFile.setEnabled(canSendFile)
        self.setWindowTitle(QString.fromUtf8(tab.getTabTitle()))
 
    def callContactSlot(self, bool):
        """ Call button was clicked. A call window will be opened.
        """
        tab = self.tabWidget.currentWidget()
        if tab != None:
            contacts = tab.getContacts()
            if len(contacts)>0:
                CallManager.getInstance().callContact(contact[0])
            else:
                tab.chatHistory.insertMessage(True, self.tr("Error"), self.tr("Cannot call %1") % tab.getTabTitle())
        self.updateActionsAndTitles(tab)

    def sendFileSlot(self, bool):
        """ Send file button was clicked. Open a send file widget.
        """
        tab = self.tabWidget.currentWidget()
        if tab != None:
            contacts = tab.getContacts()
            if len(contacts)>0:
                fileName = QFileDialog.getOpenFileName(None, self.tr("Choose the file to send"))
                FileTransferWidget.getInstance().sendFile(fileName, contacts[0])
            else:
                tab.chatHistory.insertMessage(True, self.tr("Error"), self.tr("Cannot send file to %1") % tab.getTabTitle())

    def createChatTabFromContact(self, tCoIpManager, contact):
        """
        Creates a ChatTab with the new contact. If one already exists, it shows it
        """
        tab = self.findChatTab(contact)
        if tab==None:
            # ChatTab does not exist
            log.debug("createChatTabFromContact: ChatTab created for "+str(contact.getDisplayName()))
            # Need to create a session
            session = self.createChatSession(contact, tCoIpManager)
            tab = ChatTabWidget(self, tCoIpManager, session)
            # Should use getShortDisplayName()
            self.tabWidget.addTab(tab, contact.getDisplayName())
        # Either just created or found it, will now focus it
        self.tabAdded(tab)

    def createChatTabFromSession(self, tChatSession):
        """
        Creates a ChatTab for the new incoming session.
        """
        contacts = tChatSession.getContactList()
        if len(contacts) == 0:
            tab = None
        # TODO TO FIX should we really look for an existing chatTab? the session is NEW!
        else:
            tab = self.findChatTab(contacts[0])
        if tab==None:
            # ChatTab does not exist
            log.debug("ChatTab created")
            tab = ChatTabWidget(self, self.__tCoIpManager, tChatSession)
            # TODO set tab name
            self.tabWidget.addTab(tab, tab.getTabTitle())
        # Either just created or found it, will now focus it
        self.tabAdded(tab)

    def tabAdded(self, tab):
        """ Switches focus to the new tab, activates the chat window
            Updates the window title
        """
        self.tabWidget.setCurrentWidget(tab)
        self.activateWindow()
        self.show()
        self.updateActionsAndTitles(tab)

    def createChatSession(self, contact, tCoIpManager):
        session = tCoIpManager.getTChatSessionManager().createTChatSession()
        error = session.addContact(contact)
        if error != EnumSessionError.SessionErrorNoError:
            return None
        session.start()
        return session

    def findChatTab(self, contact):
        """
        Looks for an existing ChatTabWidget containing a ChatSession with the
        given contact
        @param contact The contact we should look for
        @return The ChatTabWidget, or None if not found
        """
        # Get all the ChatTabWidgets
        chatTabs = map(self.tabWidget.widget, range(0, self.tabWidget.count()))
        # Get their contactLists
        contacts = map(ChatTabWidget.getContacts, chatTabs)
        # I redefine the == operator for Contact to compare UUID's, not object id's
        # Otherwise, built-in functions like (x in list) do not work properly
        Contact.__eq__ = lambda a,b: a.getUUID() == b.getUUID()
        for contList in contacts:
            if contact in contList:
                return chatTabs[contacts.index(contList)]
        return None

    def closeEvent(self, event):
        event.accept()
        self.cleanUpBeforeHide()

    def closeActionTriggeredSlot(self):
        self.cleanUpBeforeHide()
        self.hide()

    def cleanUpBeforeHide(self):
        while self.tabWidget.count()>0:
            self.tabWidget.removeTab(0)
            #TODO maybe also delete the object?

    def closeButtonClickedSlot(self):
        """
        Close button was clicked, must close the current tab
        We simply forward the close request to the tab
        """
        tab = self.tabWidget.currentWidget()
        if tab != None:
            tab.closeButtonClicked()
        else:
            self.close()

    def newChatSessionCreatedSlot(self, tChatSession):
        self.createChatTabFromSession(tChatSession)
