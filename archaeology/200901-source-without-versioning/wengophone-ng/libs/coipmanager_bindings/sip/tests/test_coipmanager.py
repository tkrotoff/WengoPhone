# Test script for Python binding.
# If SDK is installed in default path, use it like this:
#
# On Mac OS X: PYTHONPATH=/Library/Frameworks/CoIpManager.framework/Libraries python test_coipmanager.py
# On Linux: PYTHONPATH=/usr/local/CoIpManager-0.1/lib LD_LIBRARY_PATH=/usr/local/CoIpManager-0.1/lib QT_NO_GLIB=1 python test_coipmanager.py
# On Windows: cd C:\CoIpManager-0.1\lib && python test_coipmanager.py
#
# @author Philippe Bernery

#import PyCoIpManager
from PyCoIpManager import *
from PyQt4.QtCore import *
from PyQt4.QtGui import *


## Slots
def initializedSlot():
	## Registering to Signals
	QObject.connect(myTCoIp.getTUserProfileManager(),
		SIGNAL("userProfileSetSignal(UserProfile)"), userProfileSetSlot)
	####

	## Start
	userProfile = UserProfile()
	myTCoIp.getTUserProfileManager().setUserProfile(userProfile)
	####

def userProfileSetSlot(userProfile):
	print "** UserProfile set"
	account = Account("ovidiu.ciule@gmail.com", "kenvelo", EnumAccountType.AccountTypeWengo)
	myTCoIp.getTUserProfileManager().getTAccountManager().add(account)
	myTCoIp.getTConnectManager().connect(account.getUUID())
####


## TCoIpManager initialization
myConfig = CoIpManagerConfig()

myTCoIp = TCoIpManager(myConfig)
## Registering to Signals
QObject.connect(myTCoIp,
	SIGNAL("initialized()"), initializedSlot)
myTCoIp.start()
####

app = QApplication([])
app.exec_()
