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
from userprofile.ProfileDetails_ui import Ui_ProfileDetails

from PyCoIpManager import *

from PyQt4.Qt import *
from PyQt4.QtGui import *

import logging, os

log = logging.getLogger("PyWengoPhone.ProfileEditWidget")

class ProfileEditWidget(QDialog):
    """ Widget for editing profiles.
    """
    def __init__(self, tCoIpManager, profile, parent=None):
        QDialog.__init__(self, parent)

        self._profile = profile
        self._tCoIpManager = tCoIpManager

        ## Initializing UI
        self._ui = Ui_ProfileDetails()
        self._ui.setupUi(self)
        ####

        ## Signals
        QObject.connect(self._ui.avatarPixmapButton, SIGNAL("clicked()"), self.changeProfileAvatar)
        QObject.connect(self._ui.saveButton, SIGNAL("clicked()"), self.saveButtonClicked)
        QObject.connect(self._ui.cancelButton, SIGNAL("clicked()"), self.cancelButtonClicked)
        ####

        self.updateContent()

    def saveButtonClicked(self):
        self.saveProfile()
        self.accept()

    def cancelButtonClicked(self):
        self.reject()

    def updateContent(self):
        """ Reads content from self._profile and sets widgets values.
        """
        self._ui.firstNameLineEdit.setText(QString.fromUtf8(self._profile.getFirstName()))
        self._ui.lastNameLineEdit.setText(QString.fromUtf8(self._profile.getLastName()))

        self._ui.genderComboBox.setCurrentIndex(self._profile.getSex())

        date = self._profile.getBirthdate()
        self._ui.birthDate.setDate(QDate(date.getYear(), date.getMonth(), date.getDay()))

        address = self._profile.getStreetAddress()

        countryList = []
        for country in CountryList.getCountryList():
            countryList.append(QString.fromUtf8(country))
        self._ui.countryComboBox.clear()
        self._ui.countryComboBox.addItems(countryList)

        self._ui.countryComboBox.setCurrentIndex(self._ui.countryComboBox.findText(QString.fromUtf8(address.getCountry())))
        self._ui.cityLineEdit.setText(QString.fromUtf8(address.getCity()))
        self._ui.stateLineEdit.setText(QString.fromUtf8(address.getStateProvince()))

        self._ui.mobilePhoneLineEdit.setText(self._profile.getMobilePhone())
        self._ui.homePhoneLineEdit.setText(self._profile.getHomePhone())

        self._ui.emailLineEdit.setText(self._profile.getPersonalEmail())
        self._ui.webLineEdit.setText(self._profile.getWebsite())

        self._ui.notesEdit.setPlainText(QString.fromUtf8(self._profile.getNotes()))

        self.updateAvatarButton()

    def updateAvatarButton(self):
        backgroundPixmapFilename = ":/pics/avatar_background.png"
        foregroundPixmapData = self._profile.getIcon().getData()
        pixmap = PixmapMerging.merge(foregroundPixmapData, backgroundPixmapFilename)
        self._ui.avatarPixmapButton.setIcon(QIcon(pixmap))

    def changeProfileAvatar(self):
        avatarSelector = ImageSelector(self)
        avatarSelector.setWindowTitle(self.tr("Select your avatar"))
        startDir = os.path.join(Config.getInstance().resourcesFolder, "pics", "avatars")
        avatarSelector.setCurrentDir(startDir)

        # Add Wengo avatar dir
        iconSize = ImageSelector.START_DIR_ICON_SIZE
        wengoPixmap = QPixmap(os.path.join(startDir, "default-avatar.png"))
        wengoPixmap = wengoPixmap.scaled(iconSize, iconSize, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        avatarSelector.addStartDirItem(startDir, self.tr("@company@ Avatars"), wengoPixmap)

        # Add desktop dirs
        service = DesktopService.getInstance()
        startDirList = service.startFolderList()
        for d in startDirList:
            name = service.userFriendlyNameForPath(d)
            pix = service.pixmapForPath(d, iconSize)
            avatarSelector.addStartDirItem(d, name, pix)

        if avatarSelector.exec_() == QDialog.Accepted:
            self.setAvatarImage(avatarSelector.path())
            self.updateAvatarButton()

    def setAvatarImage(self, path):
        # Size of Wengo avatars
        size = QSize(96, 96)
        # Image is scaled to not save a big picture in userprofile.xml
        image = QImage(path)
        image = image.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)

        buf = QBuffer()
        buf.open(QIODevice.ReadWrite)
        image.save(buf, "PNG")
        buf.close()

        picture = OWPicture.pictureFromData(buf.data())
        fileInfo = QFileInfo(path)
        picture.setFilename(fileInfo.fileName().toUtf8().data())

        self._profile.setIcon(picture)
        print path

    def saveProfile(self):
        """ Saves information entered into the profile.
        """
        self._profile.setFirstName(self._ui.firstNameLineEdit.text().toUtf8().data())
        self._profile.setLastName(self._ui.lastNameLineEdit.text().toUtf8().data())

        sex = EnumSex.SexUnknown
        if self._ui.genderComboBox.currentIndex() == 1:
            sex = EnumSex.SexMale
        elif self._ui.genderComboBox.currentIndex() == 2:
            sex = EnumSex.SexFemale
        self._profile.setSex(sex)

        date = self._ui.birthDate.date()
        self._profile.setBirthdate(Date(date.day(), date.month(), date.year()))

        address = StreetAddress()
        address.setCountry(self._ui.countryComboBox.currentText().toUtf8().data())
        address.setStateProvince(self._ui.stateLineEdit.text().toUtf8().data())
        address.setCity(self._ui.cityLineEdit.text().toUtf8().data())
        self._profile.setStreetAddress(address)

        self._profile.setMobilePhone(str(self._ui.mobilePhoneLineEdit.text()))
        self._profile.setHomePhone(str(self._ui.homePhoneLineEdit.text()))

        self._profile.setPersonalEmail(str(self._ui.emailLineEdit.text()))
        self._profile.setWebsite(str(self._ui.webLineEdit.text()))

        self._profile.setNotes(self._ui.notesEdit.toPlainText().toUtf8().data())
