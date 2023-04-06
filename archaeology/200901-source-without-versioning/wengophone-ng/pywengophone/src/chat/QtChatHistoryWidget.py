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

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import logging

COLOR_ME = "#9999ff"
COLOR_PEER = "#99ff99"

log = logging.getLogger("PyWengoPhone.ChatMainWindow.ChatTabWidget.QtChatHistoryWidget")

class QtChatHistoryWidget(QTextBrowser):
    def __init__(self, parent):
        QTextBrowser.__init__(self, parent)
        self.setFrameStyle(QFrame.NoFrame | QFrame.Plain)
        self.setLineWrapMode(QTextEdit.WidgetWidth)
        self.setWordWrapMode(QTextOption.WrapAnywhere)

    def insertMessage(self, isMe, sender, message):
        if isMe:
            color = COLOR_ME
        else:
            color = COLOR_PEER
        html = QString("<table width='100%%' bgcolor='%s'><tr><td>%s</td></tr></table>" % (color, sender))
        html += QString.fromUtf8(message)
 
        # Super hackish pixel-precise vertical spacer
        html += "<div style='font-size: 6px'>&nbsp;</div>"
        self.insertHtml(html)
        self.verticalScrollBar().setValue(self.verticalScrollBar().maximum())
