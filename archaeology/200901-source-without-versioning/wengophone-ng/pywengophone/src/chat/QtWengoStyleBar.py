# -*- coding: utf-8 -*-
# vi: noexpandtab tabstop=4
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

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from util.DropDownWindow import *
from util.SafeConnect import *
from emoticons.EmoticonsWidget import *


class StyleButton(QPushButton):
    def __init__(self, parent, name):
        QPushButton.__init__(self, parent)
        self.setFocusPolicy(Qt.NoFocus)

        self._offPixmap = QPixmap(":/pics/chat/%s.png" % name)
        self._onPixmap = QPixmap(":/pics/chat/%s_on.png" % name)

        self.setAutoFillBackground(True)
        self.setFixedSize(self._offPixmap.size())
    
    def paintEvent(self, event):
        painter = QPainter(self)
        if self.isDown() or self.isChecked():
            pix = self._onPixmap
        else:
            pix = self._offPixmap
        painter.drawPixmap(0, 0, pix)


class QtWengoStyleBar(QWidget):
    def __init__(self, parent):
        QWidget.__init__(self, parent)

        self._bgPixmap = QPixmap(":/pics/chat/bar_fill.png")
        self.setFixedHeight(self._bgPixmap.height())

        self._layout = QHBoxLayout(self)
        self._layout.setMargin(0)
        self._layout.setSpacing(9) # Fix a spacing problem on Mac OS X
        self.__emoticonButton = self.createButton("chat_emoticon_button")
        self.__emoticonButton.setCheckable(True)
        # Create the emoticon drop down window
        self.__emoticonDropDownWindow = DropDownWindow(self.__emoticonButton)
        self.__emoticonDropDownWindow.setFrameStyle(QFrame.StyledPanel | QFrame.Raised)

        self.emoticonWidget = EmoticonsWidget(self.__emoticonDropDownWindow)

        layout = QVBoxLayout(self.__emoticonDropDownWindow)
        layout.addWidget(self.emoticonWidget)

        self._formatFromButton = {}
        for buttonName in ["bold", "underline", "italic"]:
            button = self.createButton("font_" + buttonName)
            button.setCheckable(True)
            self._formatFromButton[button] = buttonName
            connect(button, SIGNAL("toggled(bool)"), self.slotFormatButtonToggled)

        self._layout.addStretch()

        self.createEndLabel()

    def slotFormatButtonToggled(self, value):
        format = self._formatFromButton[self.sender()]
        self.emit(SIGNAL(format + "Toggled"), value)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.drawTiledPixmap(self.rect(), self._bgPixmap)

    def createButton(self, name):
        button = StyleButton(self, name)
        self._layout.addWidget(button)
        return button

    def createEndLabel(self):
        label = QLabel(self)
        pix = QPixmap(":/pics/chat/bar_end.png")
        label.setPixmap(pix)
        label.setAutoFillBackground(True)
        self._layout.addWidget(label)
