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


class Emoticon:
    def __init__(self):
        self._path = ""
        self._text = []
        self._pixmap = None
        self._buttonPixmap = None

    def Emoticon(self, source):
        self._path = source._path
        self._text = source._text
        self._pixmap = source._pixmap
        self._buttonPixmap = source._buttonPixmap

    def setPath(self, path):
        self._path = path

    def setPixmap(self, pix):
        self._pixmap = pix

    def setButtonPixmap(self, pix):
        self._buttonPixmap = pix

    def setText(self, txt):
        self._text = txt

    def getText(self):
        return self._text

    def getButtonPixmap(self):
        return self._buttonPixmap

    def getPixmap(self):
        return self._pixmap

    def getHtml(self):
        return "<img src=\""+self._path+"\" />"

    def isNull(self):
        return len(self._text)==0

