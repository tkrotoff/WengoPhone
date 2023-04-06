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
  Porting of SafeConnect from C++ to Python
  
  Replaces QObject::connect() and QObject::disconnect()
 
  Checks if QObject::connect() and QObject::disconnect() succeed or not.
  If not then an assertion will occur.
  
  @see QObject::connect()
  @see QObject::disconnect()
  @see <util/SafeConnect.h> 

  @author Ovidiu Ciule
"""

import sys, logging
from PyQt4 import QtCore

"""
Warning:
Only QObject.connect(sender, signal, receiver) seems to work;
The 4 parameter call mostly fails
"""
def connect(sender, signal, receiver, type = QtCore.Qt.AutoCompatConnection):
  if (not QtCore.QObject.connect(sender, signal, receiver, type)):
    logging.critical("**** Cannot connect sender=" + sender.objectName() + \
      " signal=" + signal + \
      " to receiver=" + receiver)
    sys.exit()

def disconnect(sender, signal, receiver):
  if (not QtCore.QObject.disconnect(sender, signal, receiver)):
    logging.critical("**** Cannot disconnect sender=" + sender.objectName() + \
      " signal=" + signal + \
      " from receiver=" + receiver)
    sys.exit()

"""
Doesn't seem to work with PyQt4
"""
def disconnectObject(obj):
  if (not obj.disconnect()):
    logging.critical("**** Cannot disconnect sender=" + sender.objectName() + \
      " signal=" + signal + \
      " from receiver=" + receiver)
    sys.exit()


