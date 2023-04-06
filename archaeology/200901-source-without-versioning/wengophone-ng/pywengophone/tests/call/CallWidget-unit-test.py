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
# @author Ovidiu Ciule

from call.CallWidget import *
from PyQt4.QtGui import *
import logging, unittest, sys


class TestCallWidgetClass(unittest.TestCase):
    """
    CallWidget unit tests
    """
    def testQImageFromData(self):
        width = 10
        height = 10
        data = '\x80'*width*height*4
        img = QImage(data, width, height, QImage.Format_RGB32)
        self.failUnlessEqual(len(self.getData(img)), len(data))
        self.failUnlessEqual(self.getData(img), data)

    def testIsRealWebcamVideoWithFalseFrame(self):
        width = 10
        height = 10
        data = '\x80'*width*height*4
        img = QImage(data, width, height, QImage.Format_RGB32)
        self.failUnlessEqual(len(self.getData(img)), len(data))
        self.failIf(CallWidget.isRealWebcamVideoFrame(img))

    def testIsRealWebcamVideoWithLargeFalseFrame(self):
        width = 640
        height = 480
        data = '\x80'*width*height*4
        img = QImage(data, width, height, QImage.Format_RGB32)
        self.failIf(CallWidget.isRealWebcamVideoFrame(img))

    def testIsRealWebcamVideoWithSimulatedRealFrame(self):
        data = '\x10'*400
        img = QImage(data, 10, 10, QImage.Format_RGB32)
        self.failUnless(CallWidget.isRealWebcamVideoFrame(img))

    def testIsRealWebcamVideoWithRealFrame(self):
        img = QImage()
        img.load("/home/ociule/coipmanager/wengophone/resources/pics/avatars/default-avatar.png")
        self.failUnless(CallWidget.isRealWebcamVideoFrame(img))

    def getData(self, img):
        # Used for debugging
        return img.bits().asstring(img.numBytes())

if __name__ == '__main__':
    logging.basicConfig()
    log = logging.getLogger("PyWengoPhone")
    log.setLevel(logging.DEBUG)
    unittest.main()
