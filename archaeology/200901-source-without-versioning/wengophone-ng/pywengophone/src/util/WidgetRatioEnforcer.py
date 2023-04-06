#
# WengoPhone, a voice over Internet phone
# Copyright (C) 2004-2007  Wengo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# @author Aurelien Gateau
# @author Ovidiu Ciule

from PyQt4.QtCore import *
from PyQt4.QtGui import *


class WidgetRatioEnforcer(QFrame):

    def __init__(self, parent = None):
        QFrame.__init__(self, parent)
        self.__child = None
        self.__ratio = 1

        policy = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        policy.setHeightForWidth(True)
        self.setSizePolicy(policy)

    def setRatio(self, ratio):
        if ratio!=0:
            self.__ratio = ratio
        self.updateChild()

    def setChild(self, child):
        self.__child = child
        self.updateChild()

    def sizeHint(self):
        if self.__child!=None:
            return self.__child.sizeHint()
        else:
            return QFrame.sizeHint()

    def minimumSizeHint(self):
        if self.__child:
            return self.__child.minimumSizeHint()
        else:
            return QFrame.minimumSizeHint()

    def heightForWidth(self, width):
        if self.__child:
            return -1
        if self.__ratio == 0:
            return -1
        return min(int(width / self.__ratio), self.__child.maximumHeight())

    def resizeEvent(self, event):
        self.updateChild()

    def updateChild(self):
        if self.__child == None:
            return
        if self.__ratio == 0:
            return
        parentWidth = self.parent().width()
        parentHeight = self.parent().height()
        childWidth = 0
        childHeight = int(parentWidth / self.__ratio)
        if childHeight > self.__child.maximumHeight():
            childHeight = self.__child.maximumHeight()
        if childHeight <= parentHeight:
            childWidth = int(childHeight * self.__ratio)
        else:
            childWidth = int(parentHeight * self.__ratio)
            if childWidth > self.__child.maximumWidth():
                childWidth = self.__child.maximumWidth()
            childHeight = int(childWidth / self.__ratio)
        posX = (parentWidth - childWidth) / 2
        posY = (parentHeight - childHeight) / 2
        self.__child.setGeometry(posX, posY, childWidth, childHeight)
