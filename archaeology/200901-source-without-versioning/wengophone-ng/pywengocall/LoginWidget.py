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

from SafeConnect import *
from LoginWidget_ui import Ui_LoginWidget
from CallWidget import CallWidget
from PyQt4 import QtCore, QtGui
from PyCoIpManager import *
import logging, os, sys

log = logging.getLogger("PyWengoCall.LoginWidget")


class LoginWidget(QtGui.QMainWindow):

  def __init__(self, parent=None):
    # Init GUI   
    QtGui.QWidget.__init__(self, parent)
    self._ui = Ui_LoginWidget()
    widget = QtGui.QWidget()
    self._ui.setupUi(widget)
    self.setCentralWidget(widget)

    self._accountId = None
    self.__callWidgets = []

    # The slot isn't activated if it's not connected like this: Object.slot <- no parentheses!! 
    connect(self._ui.loginButton, QtCore.SIGNAL("clicked()"), self.loginButtonClicked)
    connect(self._ui.callButton, QtCore.SIGNAL("clicked()"), self.callButtonClicked)
    connect(self._ui.realmLineEdit, QtCore.SIGNAL("editingFinished()"), self.realmLineEditingFinished)

    self._ui.loginButton.setEnabled(False)
    self._ui.callButton.setEnabled(False)

    self.statusBar().showMessage("Disconnected");

    # Adding supported protocols in combo box
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeWengo))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeSIP))
    connect(self._ui.protocolComboBox, QtCore.SIGNAL("activated(int)"), self._ui.stackedWidget, QtCore.SLOT("setCurrentIndex(int)"))

    # Add some login credentials for testing
    self._ui.wengoLoginLineEdit.setText("ovidiu.ciule@gmail.com")
    self._ui.wengoPasswordLineEdit.setText("")

    # Initializing TCoIpManager
    self._config = CoIpManagerConfig()
    self._config.addCoIpPluginsPath(os.getcwd())

    self._tCoIpManager = TCoIpManager(self._config)
    connect(self._tCoIpManager, QtCore.SIGNAL("initialized()"), self.tCoIpManagerInitializedSlot)
    self._tCoIpManager.start()

  def loginButtonClicked(self):
    if (self._accountId == None):
      try:
        self._accountId = {0: lambda: self.addWengoAccount(),
         1: lambda: self.addSipAccount()}[self._ui.stackedWidget.currentIndex()]()
      except KeyError:
        log.critical("Unknown Index")
        sys.exit()

    self._tCoIpManager.getTConnectManager().connect(self._accountId)
    self._ui.loginButton.setEnabled(False)
    self._ui.loginButton.setText("Connecting...")

  def addWengoAccount(self):
    account = Account(str(self._ui.wengoLoginLineEdit.text()),
      str(self._ui.wengoPasswordLineEdit.text()),
      EnumAccountType.AccountTypeWengo)

    self._tCoIpManager.getTUserProfileManager().getTAccountManager().add(account)
    return str(account.getUUID())

  def addSipAccount(self):
    sipAccount = SipAccount(str(self._ui.usernameLineEdit.text()),
     str(self._ui.passwordLineEdit.text()))
    sipAccount.setRealm(str(self._ui.realmLineEdit.text()))
    sipAccount.setDisplayName(str(self._ui.displaynameLineEdit.text()))
    sipAccount.setRegisterServerHostname(str(self._ui.registerServerLineEdit.text()))
    sipAccount.setSIPProxyServerHostname(str(self._ui.sipProxyLineEdit.text()))
    sipAccount.setSIPProxyServerPort(int(self._ui.sipProxyPortLineEdit.text()))
    account = Account(sipAccount)
    self._tCoIpManager.getTUserProfileManager().getTAccountManager().add(account);
    return account.getUUID() 
    

  def callButtonClicked(self):
    callWidget = CallWidget(self._tCoIpManager)
    callWidget.show()
    self.__callWidgets.append(callWidget)
    connect(callWidget, QtCore.SIGNAL("closing()"),
        lambda: self.__callWidgets.remove(callWidget))
    print len(self.__callWidgets)

  def realmLineEditingFinished(self):
    if len(str(self._ui.registerServerLineEdit.text()))==0:
      self._ui.registerServerLineEdit.setText(str(self._ui.realmLineEdit.text()))
    if len(str((self._ui.sipProxyLineEdit.text())))==0:
      self._ui.sipProxyLineEdit.setText(str(self._ui.realmLineEdit.text()))

  def tCoIpManagerInitializedSlot(self):
    connect(self._tCoIpManager.getTUserProfileManager(), QtCore.SIGNAL("userProfileSetSignal(UserProfile)"), self.userProfileSetSlot);

    connect(self._tCoIpManager.getTConnectManager(),
     QtCore.SIGNAL("accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)"),
     self.accountConnectionStateSlot)
    connect(self._tCoIpManager.getTConnectManager(),
     QtCore.SIGNAL("accountConnectionErrorSignal(std::string, EnumConnectionError::ConnectionError)"),
     self.accountConnectionErrorSlot)
    connect(self._tCoIpManager.getTConnectManager(),
     QtCore.SIGNAL("accountConnectionProgressSignal(std::string, unsigned, unsigned, std::string)"),
     self.accountConnectionProgressSlot)
    
    userProfile = UserProfile()
    self._tCoIpManager.getTUserProfileManager().setUserProfile(userProfile)

  def userProfileSetSlot(self, userProfile):
    self._ui.loginButton.setEnabled(True);

  def accountConnectionStateSlot(self, accountId, state):
    self.statusBar().showMessage(EnumConnectionState.toString(state))

    if (state == EnumConnectionState.ConnectionStateConnected):
      self._ui.callButton.setEnabled(True)
      self._ui.loginButton.setEnabled(False)
      self._ui.loginButton.setText("Connected")
    else:
      self._ui.callButton.setEnabled(False)
      self._ui.loginButton.setEnabled(True)
      self._ui.loginButton.setText("Login")

  def accountConnectionErrorSlot(self, accountId, errorCode):
    self.statusBar().showMessage(EnumConnectionError.toString(errorCode))

  def accountConnectionProgressSlot(self, accountId, currentStep, totalSteps, infoMessage):
    self.statusBar().showMessage(str(currentStep)+"/"+str(totalSteps)+" "+infoMessage)
