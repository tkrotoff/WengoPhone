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
"""


import sys
from PyQt4 import QtCore, QtGui

from LoginWidget import LoginWidget
import logging

logging.basicConfig()
log = logging.getLogger("PyWengoConnection")
log.setLevel(0)

if __name__ == "__main__":
  app = QtGui.QApplication(sys.argv)
  myapp = LoginWidget()
  myapp.show()
  sys.exit(app.exec_())
