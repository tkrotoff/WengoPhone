#!/usr/bin/env python
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
# Test program for PyCoIpManager
#
# @author Ovidiu Ciule
# @author Philippe Bernery

from config import Config
from MainWindow import MainWindow
from PyQt4.QtGui import QApplication

from getopt import getopt
import logging
import sys

logging.basicConfig()
log = logging.getLogger("PyWengoPhone")
log.setLevel(logging.DEBUG)

if __name__ == "__main__":
    app = QApplication(sys.argv)

    value, last = getopt(sys.argv[1:], "", ["resources="])
    for v in value:
        if v[0] == "--resources":
            Config.getInstance().resourcesFolder = v[1]

    MainWindow.getInstance().show()

    sys.exit(app.exec_())
