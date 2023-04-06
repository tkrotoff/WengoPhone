#
# WengoPhone, a voice over Internet phone
#
# Copyright (C) 2004-2005  Wengo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

__author__ = """David Ferlier <david.ferlier@wengo.fr"""
__doc__    = """Platform module for wengoscons"""

from wengoscons.common import wengoenv
import platform, sys, re, struct

class WengoPlatform:
    """ Platform detection routines for wengoscons """

    def WengoGetOs(self):
        """
        Returns the Operating System

        @rtype string
        """
        return sys.platform

    def WengoGetMachine(self):
        """
        Return the machine type as a string (i.e "amd64", "i386")

        @rtype string
        """

        return platform.machine()

    def WengoMachineIsAmd64(self):
        """
        Checks whether the machine is an amd64

        @rtype boolean
        """

        return self.WengoGetMachine() == "amd64"

    def WengoMachineIsI386(self):
        """
        Checks whether the machine is an i386

        @rtype boolean
        """
        return self.WengoGetMachine() == "i386"

    def WengoMachineIs32bit(self):
        """
        Checks whether this is a 32-bit machine

        @rtype boolean
        """
        return platform.architecture()[0] == "32bit"

    def WengoMachineIs64bit(self):
        """
        Checks whether this is a 64-bit machine

        @rtype boolean
        """
        return platform.architecture()[0] == "64bit"

    def WengoGetSizeOfLong(self):
        """
        Returns the size of a 'long'

        @rtype integer
        """
        return struct.calcsize("l")

    def WengoGetSizeOfInt(self):
        """
        Returns the size of an 'int'

        @rtype integer
        """
        return struct.calcsize("i")

    def WengoGetSizeOfShort(self):
        """
        Returns the size of a 'short'

        @rtype integer
        """
        return struct.calcsize("h")

    def WengoOsMatch(self, os):
        ma = re.search(os, self.WengoGetOs())
        if ma:
            return 1
        return 0

    def WengoOsIsBSD(self):
        return self.WengoOsMatch("bsd")

    def WengoOsIsLinux(self):
        return self.WengoOsMatch("linux")

    def WengoOsIsWindows(self):
        return self.WengoOsMatch("win")

    def WengoOsIsDarwin(self):
        return self.WengoOsMatch("darwin")

wengoenv.WengoInstallClass(WengoPlatform)
