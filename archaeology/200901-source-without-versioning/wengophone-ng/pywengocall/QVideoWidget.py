"""
  WengoPhone, a voice over Internet phone
  Copyright (C) 2004-2007  Wengo
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

"""
  Test program for PyCoIpManager
  
  @author Ovidiu Ciule
  @author Philippe Bernery
"""

from PyQt4 import QtGui, QtCore
from PyCoIpManager import *
from CallWidget import *

class QVideoWidget(QtGui.QLabel):
  def __init__(self, parent):
    QtGui.QLabel.__init__(self, parent)
    self._mutex = QtCore.QMutex()
    self._piximage = None
    self._QImage = None
    self._counter_image = 0
    self._parent = parent

  def setImage(self, image):
    lock = QtCore.QMutexLocker(self._mutex)
    self._QImage = image.scaled(self.size())
    self.update()

  def paintEvent(self, event):
    lock = QtCore.QMutexLocker(self._mutex)
    if (self._QImage != None):
      painter = QtGui.QPainter(self)
      painter.drawImage(0, 0, self._QImage, 0, 0, self._QImage.width(), self._QImage.height())
