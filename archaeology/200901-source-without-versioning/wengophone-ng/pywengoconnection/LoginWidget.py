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
from LogWidget import LogWidget
from PyQt4 import QtCore, QtGui
from PyCoIpManager import *
import logging, os, sys

log = logging.getLogger("PyWengoConnection.LoginWidget")

class LoginWidget(QtGui.QMainWindow):
  
  def loginButtonClicked(self):
    if (self._accountId == None):
      try:
        self._accountId = {0: lambda: self.addAccount(),
         1: lambda: self.addSipAccount()}[self._ui.stackedWidget.currentIndex()]()
      except KeyError:
        log.critical("Unknown Index")
        sys.exit()

    logWidget = LogWidget(self._tCoIpManager, self._accountId)
    self._widgetStack.append(logWidget)
    logWidget.show()
    self._ui.loginButton.setEnabled(False);

  def addAccount(self):
    accountTypeId = self._ui.protocolComboBox.currentIndex() + 1
    accountType = EnumAccountType.toAccountType(str(self._ui.protocolComboBox.currentText()))

    account = Account(str(self._ui.accountLoginLineEdit.text()),
     str(self._ui.accountPasswordLineEdit.text()),
     accountType)

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
    return str(account.getUUID())
    
  def realmLineEditingFinished(self):
    if len(str(self._ui.registerServerLineEdit.text()))==0:
      self._ui.registerServerLineEdit.setText(str(self._ui.realmLineEdit.text()))
    if len(str((self._ui.sipProxyLineEdit.text())))==0:
      self._ui.sipProxyLineEdit.setText(str(self._ui.realmLineEdit.text()))

  def setAccountType(self, accountType):
    if accountType == (EnumAccountType.toString(EnumAccountType.AccountTypeSIP)): 
      self._ui.stackedWidget.setCurrentIndex(1)
    else:
      self._ui.stackedWidget.setCurrentIndex(0)

  def tCoIpManagerInitializedSlot(self):
    connect(self._tCoIpManager.getTUserProfileManager(), QtCore.SIGNAL("userProfileSetSignal(UserProfile)"), self.userProfileSetSlot);

    userProfile = UserProfile()
    self._tCoIpManager.getTUserProfileManager().setUserProfile(userProfile)

  def userProfileSetSlot(self, userProfile):
    self._ui.loginButton.setEnabled(True);

  def __init__(self, parent=None):
    self._widgetStack = []
    # Init GUI   
    QtGui.QWidget.__init__(self, parent)
    self._ui = Ui_LoginWidget()
    widget = QtGui.QWidget()
    self._ui.setupUi(widget)
    self.setCentralWidget(widget)

    connect(self._ui.loginButton, QtCore.SIGNAL("clicked()"), self.loginButtonClicked)
    connect(self._ui.realmLineEdit, QtCore.SIGNAL("editingFinished()"), self.realmLineEditingFinished)

    self._ui.loginButton.setEnabled(False)

    # Adding supported protocols in combo box
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeAIM))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeGTalk))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeIAX))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeICQ))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeJabber))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeMSN))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeSIP))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeWengo))
    self._ui.protocolComboBox.addItem(EnumAccountType.toString(EnumAccountType.AccountTypeYahoo))
    connect(self._ui.protocolComboBox, QtCore.SIGNAL("activated(const QString&)"), self.setAccountType)

    # Add some login credentials for testing
    self._ui.accountLoginLineEdit.setText("ovidiu.ciule@gmail.com")
    self._ui.realmLineEdit.setText("voip.wengo.fr")
    self.realmLineEditingFinished()
    self._ui.usernameLineEdit.setText("ovidiu.ciule")
    

    self._accountId = None
    
    # Initializing TCoIpManager
    self._config = CoIpManagerConfig()
    self._config.addCoIpPluginsPath(os.getcwd())

    self._tCoIpManager = TCoIpManager(self._config)
    connect(self._tCoIpManager, QtCore.SIGNAL("initialized()"), self.tCoIpManagerInitializedSlot)
    self._tCoIpManager.start()
