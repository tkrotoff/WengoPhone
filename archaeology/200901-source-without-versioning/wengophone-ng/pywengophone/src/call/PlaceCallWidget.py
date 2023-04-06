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

from PlaceCallWidget_ui import Ui_PlaceCallWidget

from call import CallManager

from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.PlaceCallWidget")

class PlaceCallWidget(QWidget):
    def __init__(self, parent, tCoIpManager):
        """
        Pre-condition: tCoIpManager must be initialized
        """
        QWidget.__init__(self, parent)

        self.__tCoIpManager = tCoIpManager

        # Init GUI     
        self.__ui = Ui_PlaceCallWidget()
        self.__ui.setupUi(self)

        self._bgPixmap = QPixmap(":/pics/callbar/call_bar_fill.png")
        self.setFixedHeight(self._bgPixmap.height())

        connect(self.__ui.callButton, SIGNAL("clicked()"), self.callButtonClicked)

    def callButtonClicked(self):
        """ The Call button was clicked
        """
        CallManager.getInstance().callNumber(self.__ui.phoneNumberLineEdit.text())

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.drawTiledPixmap(self.rect(), self._bgPixmap)

