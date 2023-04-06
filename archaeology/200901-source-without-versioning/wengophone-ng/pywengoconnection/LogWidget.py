"""
  WengoPhone, a voice over Internet phone
  Copyright (C) 2004-2007  Wengo
 
  This program is free software you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

"""
  Test program for PyCoIpManager
  
  @author Ovidiu Ciule
  @author Philippe Bernery
"""

from SafeConnect import *
from LogWidget_ui import Ui_LogWidget
from PyQt4 import QtCore, QtGui
from PyCoIpManager import *
import logging, sys

log = logging.getLogger("PyWengoCall.LogWidget")

class LogWidget(QtGui.QWidget):

  def __init__(self, tCoIpManager, accountId):
    # Init GUI
    QtGui.QWidget.__init__(self, None)
    self._ui = Ui_LogWidget()
    self._ui.setupUi(self)
    self._accountId = accountId

    self._tCoIpManager = tCoIpManager

    connect(self._tCoIpManager.getTConnectManager(),
     QtCore.SIGNAL("accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)"),
     self.accountConnectionStateSlot)
    connect(self._tCoIpManager.getTConnectManager(),
     QtCore.SIGNAL("accountConnectionErrorSignal(std::string, EnumConnectionError::ConnectionError)"),
     self.accountConnectionErrorSlot)
    connect(self._tCoIpManager.getTConnectManager(),
     QtCore.SIGNAL("accountConnectionProgressSignal(std::string, unsigned, unsigned, std::string)"),
     self.accountConnectionProgressSlot)
    self._tCoIpManager.getTConnectManager().connect(accountId)

  def accountConnectionStateSlot(self, accountId, state):
    if accountId == self._accountId:
      msg = "** State change: "+str(EnumConnectionState.toString(state))
      self._ui.logTextEdit.append(msg)

  def accountConnectionErrorSlot(self, accountId, errorCode):
    if accountId == self._accountId:
      msg = "!! Error: "+EnumConnectionError.toString(errorCode)
      self._ui.logTextEdit.append(msg)

  def accountConnectionProgressSlot(self, accountId, currentStep, totalSteps, infoMessage):
    if accountId == self._accountId:
      self._ui.logTextEdit.append("** Connection Progress: step "+str(currentStep)+"/"+str(totalSteps)+" "+infoMessage)

  def closeEvent(self, closeEvent):
    self._tCoIpManager.getTConnectManager().disconnect(self._accountId)
    self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
    self.close()

