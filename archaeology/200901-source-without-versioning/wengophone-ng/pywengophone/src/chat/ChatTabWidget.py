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

from ChatTabWidget_ui import *

from emoticons import EmoticonsManager

from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.ChatTabWidget")

class ChatTabWidget(QWidget, Ui_ChatTabWidget):
    """
    ChatTabWidget: a tab in the chat window
    """
    def __init__(self, parent, tCoIpManager, session):
        QWidget.__init__(self, parent.tabWidget)

        if session == None:
            log.fatal("session cannot be None")
        self.__session = session
        self.__parent = parent
        self.__tCoIpManager = tCoIpManager
        self.__em = EmoticonsManager.getInstance()

        self.setupUi(self)
        self.initUi()

        self.messageReceivedSlot()

        # install event filters
        self.chatHistory.installEventFilter(self)
        self.chatEdit.installEventFilter(self)

        self.initAvatarFrame()
        self.initSignals()

    def initUi(self):
        pix = self.sendButton.icon().pixmap(50, 50)
        painter = QPainter(pix)
        painter.setPen(Qt.white)
        painter.drawText(pix.rect(), Qt.AlignCenter, self.tr("send"))
        painter.end()
        self.sendButton.setIcon(QIcon(pix))
        self.editActionBar.setBackgroundRole(QPalette.Base)
        self.editFrame.setBackgroundRole(QPalette.Base)

    def initAvatarFrame(self):
        # Prepare avatar pixmap
        pix = QPixmap(":/pics/avatar_background.png")
        painter = QPainter(pix)
        avatar = QPixmap(":/pics/avatars/default-avatar.png")
        avatar = avatar.scaled(64, 64, Qt.IgnoreAspectRatio, Qt.SmoothTransformation)
        painter.drawPixmap((pix.width() - avatar.width()) / 2, (pix.height() - avatar.height()) / 2, avatar)
        painter.end()

        peerLabel = QLabel(self.avatarFrame)
        peerLabel.setPixmap(pix)
        peerLabel.setFixedSize(pix.size())

        meLabel = QLabel(self.avatarFrame)
        meLabel.setPixmap(pix)
        meLabel.setFixedSize(pix.size())

        layout = QVBoxLayout(self.avatarFrame)
        layout.setMargin(0)
        layout.addWidget(peerLabel)
        layout.addStretch()
        layout.addWidget(meLabel)

        self.avatarFrame.setFixedWidth(layout.sizeHint().width())

    def showEvent(self, event):
        QWidget.showEvent(self, event)
        self.chatEdit.setFocus()

    def getTabTitle(self):
        if self.__session != None:
            contacts = self.__session.getContactList()
            if len(contacts) > 0:
                shortName = str(unicode(contacts[0].getShortDisplayName()).encode('utf-8'))
                if len(shortName)>0:
                    return shortName
                return str(unicode(contacts[0].getDisplayName()).encode('utf-8'))
        return "Unknown Session"

    def initSignals(self):
        connect(self.sendButton, SIGNAL("clicked()"), self.sendMessage)
        connect(self.editActionBar, SIGNAL("boldToggled"), self.setBold)
        connect(self.editActionBar, SIGNAL("italicToggled"), self.chatEdit.setFontItalic)
        connect(self.editActionBar, SIGNAL("underlineToggled"), self.chatEdit.setFontUnderline)
        connect(self.avatarFrameButton, SIGNAL("clicked()"), self.toggleAvatarFrame)
        connect(self.__session, SIGNAL("messageAddedSignal()"), self.messageReceivedSlot)
        connect(self.__session, SIGNAL("contactAddedSignal (Contact)"), self.contactAddedSlot)
        connect(self.__session, SIGNAL("typingStateChangedSignal(Contact, EnumChatTypingState::ChatTypingState)"),
            self.typingStateChangedSlot)
        # A bit of a hack: direct connection to internal object; should be passed through QtWengoStyleBar
        connect(self.editActionBar.emoticonWidget, SIGNAL("emoticonClicked"), self.emoticonClickedSlot)

        # Load protocol specific smileys; again, a hack, as we access directly emoticonWidget; should be passed through QtWengoStyleBar
        if self.__session != None:
            account = self.__session.getAccount()
            if account != None:
                try:
                    protocol = {EnumAccountType.AccountTypeMSN: "msn", \
                        EnumAccountType.AccountTypeYahoo: "yahoo", \
                        EnumAccountType.AccountTypeWengo: "wengo", \
                        }[self.__session.getAccount().getAccountType()]
                except KeyError:
                    protocol = "default"
                self.editActionBar.emoticonWidget.initButtons(protocol)

    def emoticonClickedSlot(self, emoticon):
        self.chatEdit.insertHtml(emoticon.getHtml())

    def toggleAvatarFrame(self):
        if self.avatarFrame.isVisible():
            self.avatarFrame.hide()
            self.avatarFrameButton.setIcon(QIcon(":pics/chat/show_avatar_frame.png"))
        else:
            self.avatarFrame.show()
            self.avatarFrameButton.setIcon(QIcon(":pics/chat/hide_avatar_frame.png"))

    def setBold(self, value):
        if value:
            self.chatEdit.setFontWeight(QFont.Bold)
        else:
            self.chatEdit.setFontWeight(QFont.Normal)

    def sendMessage(self):
        """
        Sends the message
        """
        if self.chatEdit.toPlainText().isEmpty():
            return

        html = self.chatEdit.toHtml()
        # TO DO: html cleaner
        #message = cleanHtml(html)
        message = html

        # Emoticon conversion
        # TODO: use proper protocol
        self.__em.emoticons2Text(message, "default")

        # message filtering?
        # message = self._messageFilter.run(message)

        self.chatEdit.clear()

        if self.__session != None:
            self.__session.sendMessage(str(unicode(message).encode('utf-8')))
        else:
            self.chatHistory.insertMessage(True, "Error", "Could not deliver message to peer")

        self.chatEdit.ensureCursorVisible()

    def closeButtonClicked(self):
        """
        Close button was clicked, must clean up everything:
         - close the session
         - disconnect the signals
         - close the tab
         - destroy the objects
        """
        # TODO get peer name from a proper source
        log.debug("closeButtonClicked: closingTabWidget with " + \
            str(self.__parent.tabWidget.tabText(self.__parent.tabWidget.indexOf(self)).toUtf8().data()))
        # Stop the chat session
        if self.__session != None:
            self.__session.stop()
            # Disconnect the chat typing state display signal
            QObject.disconnect(self.__session, \
                SIGNAL("typingStateChangedSignal(Contact, EnumChatTypingState::ChatTypingState)"), \
                self.typingStateChangedSlot)
            self.__session = None

        # Remove the tab from the tab widget
        ixSelf = self.__parent.tabWidget.indexOf(self)
        if ixSelf >= 0:
            self.__parent.tabWidget.removeTab(ixSelf)

        # The last tab?
        if self.__parent.tabWidget.count() == 0:
            # We should hide the ChatMainWindow!
            self.__parent.hide()

    def messageReceivedSlot(self):
        """
        Appends a new message
        """
        if self.__session!=None:
            list = self.__session.getMessageHistory()
            if len(list) > 0:
                lastMsg = list[len(list) - 1]
                # TODO: use proper protocol
                message = self.__em.text2Emoticons(lastMsg.getMessage(), "default")
                self.chatHistory.insertMessage(self.peerIsMe(lastMsg.getPeer()), str(unicode(lastMsg.getPeer().getPeerId()).encode('utf-8')), message)
                log.debug("messageReceivedSlot from: " + str(lastMsg.getPeer().getPeerId()) + " message:" + str(lastMsg.getMessage()))

    def peerIsMe(self, peer):
        """
        Returns true if the peer that sent the message is the local user
        """
        # TODO TO FIX: peer.getUUID returns a different UUID than the local account
        if self.__session!=None and self.__session.getAccount()!=None:
            myUUID = self.__session.getAccount().getUUID()
            peerUUID = peer.getUUID()
            #log.debug("peerIsMe: peer.getUUID()="+str(peerUUID)+" peerId="+str(peer.getPeerId())+" getAccount().getUUID()="+str(myUUID))
            return myUUID == peerUUID
        log.error("peerIsMe: getAccount()==None")
        return False

    def eventFilter(self, obj, event):
        if obj is self.chatHistory and event.type() == QEvent.KeyPress:
            return self.historyKeyPressEventFilter(event)
        if obj is self.chatEdit and event.type() == QEvent.KeyPress:
            return self.editKeyPressEventFilter(event)
        return False

    def historyKeyPressEventFilter(self, event):
        if event.text().size() > 0 and event.text().at(0).isPrint() and self.chatEdit.isEnabled():
            self.chatEdit.setFocus()
            newEvent = QKeyEvent(event)
            QApplication.postEvent(self.chatEdit, newEvent)
            return True
        return False

    def editKeyPressEventFilter(self, event):
        key = event.key()
        if key == Qt.Key_PageUp or key == Qt.Key_PageDown:
            newEvent = cloneKeyEvent(event)
            QApplication.postEvent(self.chatHistory, newEvent)
            return True

        # Send message with Enter key, unless a modifier is pressed
        if (key == Qt.Key_Enter or key == Qt.Key_Return) and \
            (event.modifiers() == Qt.NoModifier or event.modifiers() == Qt.KeypadModifier):
            self.sendMessage()
            return True

        realCtrlPressed = event.modifiers() & Qt.ControlModifier

        if key == Qt.Key_Tab and realCtrlPressed:
            self.emit(SIGNAL("ctrlTabPressed"))
            return True

        return False

    def typingStateChangedSlot(self, contact, state):
        action = {EnumChatTypingState.ChatTypingStateNotTyping: " is not typing", \
            EnumChatTypingState.ChatTypingStateTyping: " is typing", \
            EnumChatTypingState.ChatTypingStateStopTyping: " has stopped typing" \
            }[state]
        # TODO use getShortDisplayName
        message = contact.getDisplayName() + action
        self.__parent.statusBar().showMessage(message, 5000)

    def getContacts(self):
        """
        Returns the contacts in this chat.
        """
        return self.__session.getContactList()

    def contactAddedSlot(self, contact):
        log.debug("contactAddedSlot: contact = " + str(contact.getDisplayName()))
