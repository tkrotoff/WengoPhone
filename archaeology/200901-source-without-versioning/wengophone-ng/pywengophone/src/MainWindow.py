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

from account import AccountListManagerWidget
from config import Config
from contact import ContactListWidget
from call import PlaceCallWidget
from filetransfer import FileTransferWidget
from WengoPhoneWindow_ui import Ui_WengoPhoneWindow
from chat.ChatMainWindow import ChatMainWindow
from call.CallManager import CallManager
from sms import SMSWidget
from userprofile import UserProfileListManagerWidget
import wengophone_rc

from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import sys, os
import logging

log = logging.getLogger("PyWengoPhone.MainWindow")

class MainWindow(QMainWindow):
    """
    Main Window and object of PyWengoPhone
    Implements Singleton pattern via getInstance()
    """

    def getInstance():
        if MainWindow.__currentInstance==None:
            MainWindow.__currentInstance = MainWindow()
        return MainWindow.__currentInstance
    getInstance = staticmethod(getInstance)

    __currentInstance = None

    def __init__(self):
        """
        Constructor implements Singleton pattern
        """
        if MainWindow.__currentInstance!=None:
            # Should not get here, constructor should not be called directly
            raise RuntimeError, "Constructor has been called twice. Probably \
                it has been called directly.\n This is a Singleton. Please \
                use getInstance()"
            return
        QMainWindow.__init__(self, None)

        # Setting up ui
        self.__ui = Ui_WengoPhoneWindow()
        self.__ui.setupUi(self)
        ####

        connect(self.__ui.actionQuit, SIGNAL("triggered()"), self.close)
        connect(self.__ui.actionMyProfiles, SIGNAL("triggered()"), self.showUserProfileListManager)
        connect(self.__ui.actionMyAccounts, SIGNAL("triggered()"), self.showAccountListManager)
        connect(self.__ui.actionSendSms, SIGNAL("triggered()"), self.showSmsWidget)

        self.__ui.actionMyProfiles.setEnabled(False)
        self.__ui.actionMyAccounts.setEnabled(False)
        self.__ui.actionSendSms.setEnabled(False)

        self.__initTCoIpManager()

    def closeEvent(self, event):
        self.__tCoIpManager.release()
        Config.getInstance().save()
        QMainWindow.closeEvent(self, event)

    def __initTCoIpManager(self):
        """
        Initializes TCoIpManager.
        """
        # Initializing TCoIpManager
        self.setStatusMessage("Setting TCoIpManager config...")
        tcoipconfig = CoIpManagerConfig()
        tcoipconfig.addCoIpPluginsPath(os.getcwd())
        print Config.getInstance().configurationFolder
        tcoipconfig.setConfigPath(Config.getInstance().configurationFolder)

        self.setStatusMessage("Creating TCoIpManager...")
        self.__tCoIpManager = TCoIpManager(tcoipconfig)
        if self.__tCoIpManager!= None:
            connect(self.__tCoIpManager, SIGNAL("initialized()"), self.tCoIpManagerInitializedSlot)
            self.setStatusMessage("Starting TCoIpManager...")
            self.__tCoIpManager.start()
        else:
            log.fatal("__initTCoIpManager: could not create TCoIpManager, aborting")

    def setStatusMessage(self, msg):
        """
        Sets the text of the status bar.
        """
        self.statusBar().showMessage(msg)

    def tCoIpManagerInitializedSlot(self):
        self.setStatusMessage("TCoIpManager initialized")

        connect(self.__tCoIpManager.getTUserProfileManager(),
            SIGNAL("userProfileSetSignal(UserProfile)"),
            self.userProfileSetSlot)

        connect(self.__tCoIpManager.getTConnectManager(),
            SIGNAL("accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)"),
            self.accountConnectionStateSlot)
        connect(self.__tCoIpManager.getTConnectManager(),
            SIGNAL("accountConnectionErrorSignal(std::string, EnumConnectionError::ConnectionError)"),
            self.accountConnectionErrorSlot)
        connect(self.__tCoIpManager.getTConnectManager(),
            SIGNAL("accountConnectionProgressSignal(std::string, unsigned, unsigned, std::string)"),
            self.accountConnectionProgressSlot)

        #connect(self.__tCoIpManager.getTChatSessionManager(),
        #    SIGNAL("newChatSessionCreatedSignal(TChatSession * tChatSession)"),
        #    self.accountConnectionProgressSlot)

        userProfile = UserProfile()

        ## Loading last UserProfile
        lastProfile = Config.getInstance().lastUsedUserProfile
        if lastProfile != None:
            self.setStatusMessage("Loading last used UserProfile...")
            fileStorage = UserProfileFileStorage(Config.getInstance().configurationFolder)

            if not fileStorage.load(lastProfile, userProfile) == UserProfileFileStorage.UserProfileStorageErrorNoError:
                logging.error("cannot load profile")
        else:
            pass # TO DO: ask user to choose a profile
        ####
        FileTransferWidget.createInstance(self.__tCoIpManager)

        self.setStatusMessage("Setting UserProfile...")
        self.__tCoIpManager.getTUserProfileManager().setUserProfile(userProfile)

    def userProfileSetSlot(self, userProfile):
        Config.getInstance().lastUsedUserProfile = userProfile.getUUID()

        if len(self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getAccountList()) > 0:
            self.setStatusMessage("Launching account connection...")
            self.__tCoIpManager.getTConnectManager().connectAllAccounts()

        ## Initializing sub-widgets
        cw = self.__ui.tabContactList
        bl = QVBoxLayout()
        cw.setLayout(bl)
        cl = ContactListWidget(self, self.__tCoIpManager)
        connect(self.__ui.actionAddContact, SIGNAL("triggered( bool )"), \
            lambda bool: cl.addContactClicked())
        bl.setMargin(0)
        bl.addWidget(cl)
        callFrame = self.__ui.callBarFrame
        QHBoxLayout(callFrame).setMargin(0)
        callFrame.layout().addWidget(PlaceCallWidget(self, self.__tCoIpManager))
        ####

        # TODO: create account if account does not exist
        # An Account exists?
        # ac_list = userProfile.getAccountList()
        """
        if len(ac_list)==0:
             # Create account
             self.__newAccountWidget = NewAccountWidget(self)
             self.__newAccountWidget.show()
        else:
             pass # TO DO: connect existing accounts
        """
        self.__ui.actionMyProfiles.setEnabled(True)
        self.__ui.actionMyAccounts.setEnabled(True)
        self.__ui.actionSendSms.setEnabled(True)

        # Init ChatMainWindow
        chatMainWindow = ChatMainWindow.getInstance()
        chatMainWindow.initSignals(self.__tCoIpManager)

        # Init CallManager
        callManager = CallManager.getInstance()
        callManager.initSignals(self.__tCoIpManager)

        self.setStatusMessage("UserProfile set.")

    def accountConnectionStateSlot(self, accountId, state):
        """
        Receives and displays connection state events
        """
        self.statusBar().showMessage(EnumConnectionState.toString(state).title())

    def accountConnectionErrorSlot(self, accountId, errorCode):
        self.statusBar().showMessage(EnumConnectionError.toString(errorCode).title())

    def accountConnectionProgressSlot(self, accountId, currentStep, totalSteps, infoMessage):
        self.statusBar().showMessage(str(currentStep)+"/"+str(totalSteps)+" "+infoMessage.title())

    def showUserProfileListManager(self):
        self._profMng = UserProfileListManagerWidget(self, self.__tCoIpManager)
        self._profMng.show()

    def showAccountListManager(self):
        self._accMng = AccountListManagerWidget(self, self.__tCoIpManager)
        self._accMng.show()

    def showSmsWidget(self):
        self._smsWidget = SMSWidget(self.__tCoIpManager)
        self._smsWidget.show()
