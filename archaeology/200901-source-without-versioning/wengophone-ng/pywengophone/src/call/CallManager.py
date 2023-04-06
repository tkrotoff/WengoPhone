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

from CallWidget import CallWidget
from util.SafeConnect import *

from PyCoIpManager import *

from PyQt4.QtCore import *
import logging, sys

log = logging.getLogger("PyWengoPhone.CallManager")


class CallManager:
    """
    CallManager of PyWengoPhone
    Listens to incoming calls and keeps a list of existing calls
    Implements Singleton pattern via getInstance()
    """

    def getInstance():
        if CallManager.__currentInstance==None:
            CallManager.__currentInstance = CallManager()
        return CallManager.__currentInstance
    getInstance = staticmethod(getInstance)

    __currentInstance = None

    def __init__(self):
        """
        Constructor implements Singleton pattern
        """
        if CallManager.__currentInstance!=None:
            # Should not get here, constructor should not be called directly
            raise RuntimeError, "Constructor has been called twice. Probably \
                it has been called directly.\n This is a Singleton. Please \
                use getInstance()"
            return
        ## List of existing callWidgets; Used for memory management
        self.callWidgets = []
        self.__tCoIpManager = None

    def initSignals(self, tCoIpManager):
        """
        Listen to the newCallSessionCreatedSession
        """
        connect(tCoIpManager.getTCallSessionManager(), \
            SIGNAL("newCallSessionCreatedSignal (TCallSession *)"),
            self.newCallSessionCreatedSlot)
        self.__tCoIpManager = tCoIpManager

    def newCallSessionCreatedSlot(self, session):
        """
        Receives an incoming call session
        Creates the GUI
        """
        log.debug("newCallSessionCreatedSlot(): incoming call session")
        callWidget = CallWidget(self.__tCoIpManager, self)
        callWidget.show()
        callWidget.receiveCall(session)

    def callContact(self, contact):
        callWidget = CallWidget(self.__tCoIpManager, self)
        callWidget.show()
        callWidget.callContact(contact)

    def callNumber(self, number):
        self.callWidget = CallWidget(self.__tCoIpManager, self)
        self.callWidget.show()
        self.callWidget.callNumber(number)
