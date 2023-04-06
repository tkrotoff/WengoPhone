#! /usr/bin/python
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

import sys, os, logging
sys.path.append(os.path.normpath(os.getcwd()+"/../../../src/"))
print os.path.normpath(os.getcwd()+"/../../../src/")
from util import DropDownWindow
from util.SafeConnect import *
from chat.emoticons.EmoticonsWidget import EmoticonsWidget

from PyQt4.QtCore import *
from PyQt4.QtGui import *

logging.basicConfig()
log = logging.getLogger("PyWengoPhone")
log.setLevel(logging.DEBUG)

class TestWindow(QWidget):
    """
    Functional test for DropDownWindow, EmoticonsWidget and EmoticonsManager
    ! This is not a unit test
    """
    def __init__(self):
        QWidget.__init__(self, None)
        self.__button = QToolButton(self)
        self.__button.setText("&Click Me")
        self.__button.setCheckable(True)

        self.__history = QTextEdit(self)
        self.__edit = QTextEdit(self)

        self.__window = DropDownWindow(self.__button)
        self.__window.setFrameStyle(QFrame.StyledPanel | QFrame.Raised)

        emotWidget = EmoticonsWidget(self.__window, "default")

        layout = QVBoxLayout(self.__window)
        layout.addWidget(emotWidget)

        layout2 = QVBoxLayout(self)
        layout2.addWidget(self.__button)
        layout2.addWidget(self.__history)
        layout2.addWidget(self.__edit)

        connect(emotWidget, SIGNAL("emoticonClicked"), self.emoticonClicked)

    def emoticonClicked(self, emoticon):
        self.__edit.insertHtml(emoticon.getHtml())

if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = TestWindow()
    window.show()

    sys.exit(app.exec_())

