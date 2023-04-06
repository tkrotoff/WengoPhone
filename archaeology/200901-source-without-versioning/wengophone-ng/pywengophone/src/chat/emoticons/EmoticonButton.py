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

from Emoticon import Emoticon
from util.SafeConnect import *
from PyQt4.QtGui import *
from PyQt4.QtCore import *

class EmoticonButton(QToolButton):
    """
    Custom Emoticon button. Keeps a reference to the embedded emoticon. This
    makes recovering the emoticon much easier
    """
    def __init__(self, parent, emoticon):
        QToolButton.__init__(self, parent)
        self.setAutoRaise(True)
        connect(self, SIGNAL("clicked()"), self.buttonClickedSlot)
        self.__emoticon = emoticon
        self.setIcon(QIcon(emoticon.getButtonPixmap()))
        self.setIconSize(emoticon.getButtonPixmap().size())
        if not len(emoticon.getText())==0:
            self.setToolTip(emoticon.getText()[0])
 
    def buttonClickedSlot(self):
        self.emit(SIGNAL("buttonClicked"), self.__emoticon)
