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
from NewProfileDialog import NewProfileDialog
from UserProfileListManagerWidget_ui import Ui_UserProfileListManagerWidget

from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging, os, sys

log = logging.getLogger("PyWengoPhone.UserProfileListManagerWidget")

class UserProfileListManagerWidget(QDialog):
    def __init__(self, parent, tCoIpManager):
        """
        Pre-condition: tCoIpManager must be initialized, otherwise Config would not be useful
        """
        self.__tCoIpManager = tCoIpManager

        # Init GUI     
        QDialog.__init__(self, parent)
        self._ui = Ui_UserProfileListManagerWidget()
        self._ui.setupUi(self)
        self.setAttribute(Qt.WA_DeleteOnClose)

        connect(self._ui.addButton, SIGNAL("clicked()"), self.addButtonClicked)
        connect(self._ui.loadButton, SIGNAL("clicked()"), self.loadButtonClicked)
        connect(self._ui.modifyButton, SIGNAL("clicked()"), self.editButtonClicked)
        connect(self._ui.closeButton, SIGNAL("clicked()"), self.close)
        connect(self.__tCoIpManager.getTUserProfileManager(),
            SIGNAL("userProfileSetSignal(UserProfile)"),
            self.userProfileSetUpdatedSlot)
        connect(self.__tCoIpManager.getTUserProfileManager(),
            SIGNAL("userProfileUpdatedSignal(UserProfile)"),
            self.userProfileSetUpdatedSlot)


        self._ui.deleteButton.setEnabled(False)
        self.loadUserProfileList()

    def addButtonClicked(self):
        # Show NewProfileDialog
        npw = NewProfileDialog(self)
        if npw.exec_() == QDialog.Accepted:
            up = UserProfile()
            up.setName(str(npw.getUserProfileName()))
            self.__tCoIpManager.getTUserProfileManager().setUserProfile(up)
            log.info("Profile: " + up.getName() + " id: " + up.getUUID())

    def loadUserProfileList(self):
        """
        Loads the list of existing profiles into the Widget
        For each profiles, also appends its Accounts as children
        Pre-condition: a current UP is already set
        """
        self._ui.treeWidget.clear()

        fileStorage = UserProfileFileStorage(Config.getInstance().configurationFolder)
        idList = fileStorage.getUserProfileIds()
        currentUP = self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile()
        currentId = currentUP.getUUID()
        ## Is the current UP among the stored profiles? If not, add it; this could happen if the 
        ## current UP is newly created, and has not been saved yet
        if not currentId in idList:
            idList.append(currentId)
        for id in idList:
            sl = QStringList()
            up = None
            if currentId==id:
                sl.append("*")
                up = currentUP
                profileName = up.getName()
            else:
                sl.append("")
                up = UserProfile()
                fileStorage.load(id, up)
                profileName = fileStorage.getNameOfUserProfile(id)
            if profileName==id or len(profileName)==0:    
                sl.append("<No Name>")
            else:
                sl.append(fileStorage.getNameOfUserProfile(id))
            sl.append(id)
            qTWI = QTreeWidgetItem(sl)
            self._ui.treeWidget.addTopLevelItem(qTWI)
            # Append the accounts
            for account in up.getAccountList():
                sl = QStringList("")
                login = account.getLogin()
                if len(login)==0:
                    sl.append("<No Name>")
                else:
                    sl.append(login)
                sl.append(account.getUUID())
                qTWI.addChild(QTreeWidgetItem(sl)) 

    def loadButtonClicked(self):
        profileId = self.getSelectedUserProfileId() 
        userProfile = UserProfile()
        fileStorage = UserProfileFileStorage(Config.getInstance().configurationFolder)
        fileStorage.load(profileId, userProfile)
        self.__tCoIpManager.getTUserProfileManager().setUserProfile(userProfile)
        log.info("Loaded profile: " +userProfile.getUUID())

    def editButtonClicked(self):
        profileId = self.getSelectedUserProfileId() 
        fileStorage = UserProfileFileStorage(Config.getInstance().configurationFolder)
        # Check if the profile to be modified is the current profile
        if profileId == self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getUUID():
            # If yes, just get it
            userProfile = self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile()
        else:
            # If no, load it
            userProfile = UserProfile()
            fileStorage.load(profileId, userProfile)
        log.info("Modify profile: "+ userProfile.getUUID())

        # Show NewProfileDialog, we'll use it as an 'edit profile' though
        npw = NewProfileDialog(self)
        npw.setWindowTitle("Edit User Profile")
        npw._ui.profileNameLineEdit.setText(userProfile.getName())
        if npw.exec_() == QDialog.Accepted:
            # Here we really modify the profile
            userProfile.setName(str(npw.getUserProfileName()))
            # Save the modified profile
            # Check if the profile to be modified is the current profile
            if userProfile.getUUID() == self.__tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getUUID():
                self.__tCoIpManager.getTUserProfileManager().updateUserProfile(userProfile)
            else:
                fileStorage.save(userProfile)
                self.loadUserProfileList()

    def getSelectedUserProfileId(self):
        """
        Returns the profile id corresponding to the currently selected item

        Pre-condition:
        Profile items must be in the same order as the AccountList returned by UserProfileFileStorage.getUserProfileIds()
        """
        # TODO fix it so that it works regardless of the order
        profileItem = self._ui.treeWidget.currentItem()
        ixProfile = self._ui.treeWidget.indexOfTopLevelItem(profileItem)
        fileStorage = UserProfileFileStorage(Config.getInstance().configurationFolder)
        profileId = fileStorage.getUserProfileIds()[ixProfile]
        return profileId
 
    def userProfileSetUpdatedSlot(self, userProfile):
        self.loadUserProfileList()
