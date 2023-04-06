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

from IMContactManagerWidget import IMContactManagerWidget
from profile import ProfileEditWidget

from PyQt4.Qt import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.ContactEditWidget")

class ContactEditWidget(ProfileEditWidget):
    """ Widget for editing contacts.
    """
    def __init__(self, contact, tCoIpManager, parent=None):
        ProfileEditWidget.__init__(self, tCoIpManager, contact, parent)

        ## Initializing UI
        imContactManagerWidget = IMContactManagerWidget(contact, tCoIpManager, self)
        index = self._ui.imStackedWidget.addWidget(imContactManagerWidget)
        self._ui.imStackedWidget.setCurrentIndex(index);
        ####

        # Avatar of contacts cannot be edited
        self._ui.avatarPixmapButton.setEnabled(False)

    def updateContent(self):
        self._ui.groupsLineEdit.clear()
        self._ui.groupsLineEdit.setText(QString.fromUtf8(" ".join(self._profile.getGroupList())))

        ProfileEditWidget.updateContent(self)

    def saveProfile(self):
        contactGroupList = self._profile.getGroupList()
        enteredGroupList = self._ui.groupsLineEdit.text().toUtf8().data().split()
        if contactGroupList != enteredGroupList:
            # Groups changed
            self._profile.setGroupList(enteredGroupList)

        ProfileEditWidget.saveProfile(self)
