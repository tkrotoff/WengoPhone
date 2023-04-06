# -*- coding: utf-8 -*-
#
# WengoPhone, a voice over Internet phone
# Copyright (C) 2004-2007 Wengo
#
# This program is free software you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with self program if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# @author Aurelien Gateau
# @author Ovidiu Ciule

from EmoticonsManager import *
from EmoticonButton import *
from Emoticon import *
from util.SafeConnect import *
from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging

log = logging.getLogger("PyWengoPhone.EmoticonsWidget")

class EmoticonsWidget(QWidget):
    """
    Emoticons widget
    """
    def __init__(self, parent, protocol = "default"):
        QWidget.__init__(self, parent)
        self.initLayout()
	self.__parent = parent
        self.initButtons(protocol)

    def emoticonClickedSlot(self, emoticon):
        """
        Centralizes the emoticon clicked signals
        """
	log.info("emoticonClicked"+emoticon.getText()[0])
	self.__parent.close()
	self.emit(SIGNAL("emoticonClicked"), emoticon)

    def initLayout(self):
        self.__layout = QGridLayout(self)
        self.__layout.setMargin(0)
        self.__layout.setSpacing(0)
 
    def initButtons(self, protocol):
        """
        Loads the emoticons of the protocol
        """
        for child in self.__layout.children():
            self.__layout.removeWidget(child)
            del(child)
        self.__buttonX = 0
        self.__buttonY = 0
        emoticonsManager = EmoticonsManager.getInstance()
        emoticonList = emoticonsManager.getEmoticonList(protocol)
        for emoticon in emoticonList: 
            self.addButton(emoticon)
        
    def addButton(self, emoticon):
        """
        Adds a button; uses a grid 'layout'
        """
        if (self.__buttonX == 10):
                self.__buttonX = 0
                self.__buttonY += 1
        
        button = EmoticonButton(self, emoticon)
        self.__layout.addWidget(button, self.__buttonY, self.__buttonX)
        connect(button, SIGNAL("buttonClicked"), self.emoticonClickedSlot)
        self.__buttonX += 1
