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
# @author Ovidiu Ciule

import sys, os

from PyQt4.QtCore import *
from PyQt4.QtGui import *

class TestWindow(QMainWindow):
    """
    Functional test for QTabWidget:setCornerWidget
    ! This is not a unit test
    """
    def __init__(self):
        QMainWindow.__init__(self, None)
        tabWidget = QTabWidget(self)

        tab1 = QFrame(tabWidget)
        label = QLabel(tab1)
        label.setText("Some random content")
        layouttab1 = QVBoxLayout(tab1)
        layouttab1.addWidget(label)
        tabWidget.addTab(tab1, "Tab 1")

        tab2 = QFrame(tabWidget)
        checkBox = QCheckBox(tab2)
        checkBox.setText("A check box")
        layouttab2 = QVBoxLayout(tab2)
        layouttab2.addWidget(checkBox)
        tabWidget.addTab(tab2, "Tab 2")

        button = QToolButton(tabWidget)
        tabWidget.setCornerWidget(button)
        #layout = QVBoxLayout(self)
        self.setCentralWidget(tabWidget)

if __name__ == "__main__":
    app = QApplication(sys.argv)

    window = TestWindow()
    window.show()

    sys.exit(app.exec_())

