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

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from PyCoIpManager import *

import logging, os, sys

log = logging.getLogger("PyWengoPhone.EditAccountDialog.GTalkSettingsWidget")

class GTalkSettingsWidget(QWidget):
    """
      GTalk Settings Widget
      Each Account type (SIP, Wengo, GTalk etc.) has specific settings, 
      therefore they use specific GUI and code
    """

    def __init__(self, parent, account):
        # Init GUI     
        QWidget.__init__(self, parent)

    def modifyAccount(self, account):
        return account
