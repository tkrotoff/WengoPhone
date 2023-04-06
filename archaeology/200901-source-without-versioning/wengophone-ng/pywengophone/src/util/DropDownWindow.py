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
# along with this program if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
# @author Aurelien Gateau
# @author Ovidiu Ciule

from SafeConnect import *
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import platform

class DropDownWindow(QFrame):
    """
    This is a pop-up window that appears under/above a button
    Used for the smileys pop-up selection menu in the chat window, among other
    uses
    """
    def __init__(self, button):
        self.__button = button
        QFrame.__init__(self, button, Qt.Popup | Qt.FramelessWindowHint)
        connect(button, SIGNAL("toggled(bool)"), self.buttonToggledSlot)
    
    def showEvent(self, event):
        rect = QApplication.desktop().availableGeometry(self.__button)
        windowWidth = self.sizeHint().width()
        windowHeight = self.sizeHint().height()

        point = self.__button.mapToGlobal(QPoint(0, 0))

        # Make sure the dialog fit in the screen horizontally
        if (point.x() + windowWidth > rect.right()):
            point.setX(point.x() + self.__button.width() - windowWidth)
    
        # Make sure the dialog fit in the screen vertically
        if (point.y() + self.__button.height() + windowHeight > rect.bottom()):
            point.setY(point.y() - windowHeight)
        else:
            point.setY(point.y() + self.__button.height())

        self.move(point)

    def hideEvent(self, event):
        QFrame.hideEvent(self, event)
        # This is a bit hackish: the window hides itself when one clicks outside it
        # (because it has the Qt.Popup flag). In this case we want to uncheck its
        # associated button.

        # On MacOS X we must always uncheck the button ourself.
        # On Linux and Windows, we must not uncheck it ourself if the cursor is
        # over the button, because the button receives the click and will reopen
        # itself.

        if not self.__button.isChecked(): 
            return
    
        #ifndef OS_MACOSX
        if not platform.system()=="Darwin":
        # Note: Do not use self.__button.underMouse(), because it returns False if the
        # user has clicked on a widget inside the DropDownWindow instance.
            pos = self.__button.mapFromGlobal(QCursor.pos())
            underMouse = self.__button.rect().contains(pos)
            if underMouse: 
                return
    
        #endif

        self.__button.blockSignals(True)
        self.__button.setChecked(False)
        self.__button.blockSignals(False)

    def buttonToggledSlot(self, on):
        if on:
            self.show()
        else:
            self.hide()
    
