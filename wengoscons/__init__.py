#
# WengoSCons, a high-level library for SCons
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

import SCons.Script
from wengoscons import wengoenv

#Global functions

try:
	gdict = SCons.Script._SConscript.GlobalDict
except AttributeError:    
	gdict =  SCons.Script.SConscript.GlobalDict

gdict['WengoGetEnvironment'] = wengoenv.WengoGetEnvironment
gdict['WengoGetQt3Environment'] = wengoenv.WengoGetQt3Environment
gdict['WengoGetQt4Environment'] = wengoenv.WengoGetQt4Environment

gdict['WengoGetRootBuildDir'] = wengoenv.WengoGetRootBuildDir

gdict['WengoCCGCC'] = wengoenv.WengoCCGCC
gdict['WengoCCMinGW'] = wengoenv.WengoCCMinGW
gdict['WengoCCMSVC'] = wengoenv.WengoCCMSVC

gdict['WengoOSWindows'] = wengoenv.WengoOSWindows
gdict['WengoOSWindowsCE'] = wengoenv.WengoOSWindowsCE
gdict['WengoOSLinux'] = wengoenv.WengoOSLinux
gdict['WengoOSMacOSX'] = wengoenv.WengoOSMacOSX
gdict['WengoOSBSD'] = wengoenv.WengoOSBSD
gdict['WengoOSPosix'] = wengoenv.WengoOSPosix

gdict['WengoArchX86'] = wengoenv.WengoArchX86
gdict['WengoArchPPCMacintosh'] = wengoenv.WengoArchPPCMacintosh

gdict['WengoGetConsoleArgument'] = wengoenv.WengoGetConsoleArgument
gdict['WengoAddConsoleArguments'] = wengoenv.WengoAddConsoleArguments

gdict['WengoShowWindowsConsole'] = wengoenv.WengoShowWindowsConsole

gdict['WengoSetVariable'] = wengoenv.WengoSetVariable
gdict['WengoGetVariable'] = wengoenv.WengoGetVariable

gdict['WengoDebugMode'] = wengoenv.WengoDebugMode
gdict['WengoReleaseMode'] = wengoenv.WengoReleaseMode
gdict['WengoReleaseSymbolsMode'] = wengoenv.WengoReleaseSymbolsMode

gdict['WengoGetSubversionRevision'] = wengoenv.WengoGetSubversionRevision
gdict['WengoGetCurrentDateTime'] = wengoenv.WengoGetCurrentDateTime

gdict['WengoAddCCFlags'] = wengoenv.WengoAddCCFlags
gdict['WengoAddDefines'] = wengoenv.WengoAddDefines
gdict['WengoAddIncludePath'] = wengoenv.WengoAddIncludePath
gdict['WengoPrependIncludePath'] = wengoenv.WengoPrependIncludePath
gdict['WengoAddLibPath'] = wengoenv.WengoAddLibPath
gdict['WengoPrependLibPath'] = wengoenv.WengoPrependLibPath

gdict['WengoGetSourcePath'] = wengoenv.WengoGetSourcePath
