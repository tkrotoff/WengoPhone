# 
# WengoPhone, a voice over Internet phone
#
# Copyright (C) 2004-2005  Wengo
# Copyright (C) 2005 David Ferlier <david.ferlier@wengo.fr>
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

#from SCons.Script import SConscript

import SCons.Script

from wengoscons.common import wengoenv
from wengoscons.common import cppdefines
from wengoscons.common import includepaths
from wengoscons.common import libraries
from wengoscons.common import ccflags
from wengoscons.common import linkflags
from wengoscons.common import libdec
from wengoscons.common import confighwriter
from wengoscons.common import vcproj
from wengoscons.common import wplatform
from wengoscons import qtsupport
from wengoscons.qtsupport import qtenv, images, language, qt4

# Defines functions

wengoenv.WengoSConsEnvironment.WengoCppDefineAdd = cppdefines.WengoCppDefineAdd
wengoenv.WengoSConsEnvironment.WengoCppDefineSet = cppdefines.WengoCppDefineSet
wengoenv.WengoSConsEnvironment.WengoCppDefineSetMultiple = cppdefines.WengoCppDefineSetMultiple
wengoenv.WengoSConsEnvironment.WengoCppDefineRemove = cppdefines.WengoCppDefineRemove

# Include paths functions

wengoenv.WengoSConsEnvironment.WengoIncludePathsAdd = includepaths.WengoIncludePathsAdd
wengoenv.WengoSConsEnvironment.WengoIncludePathsAddMultiple = includepaths.WengoIncludePathsAddMultiple
wengoenv.WengoSConsEnvironment.WengoIncludePathsRemove = includepaths.WengoIncludePathsRemove

# Library paths functions

wengoenv.WengoSConsEnvironment.WengoLibPathAdd = libraries.WengoLibPathAdd
wengoenv.WengoSConsEnvironment.WengoLibPathAddMultiple = libraries.WengoLibPathAddMultiple
wengoenv.WengoSConsEnvironment.WengoLibPathRemove = libraries.WengoLibPathRemove

# Library functions
wengoenv.WengoSConsEnvironment.WengoLibAdd = libraries.WengoLibAdd
wengoenv.WengoSConsEnvironment.WengoLibAddMultiple = libraries.WengoLibAddMultiple
wengoenv.WengoSConsEnvironment.WengoLibRemove = libraries.WengoLibRemove

# CCFlags

wengoenv.WengoSConsEnvironment.WengoCCFlagsAdd = ccflags.WengoCCFlagsAdd
wengoenv.WengoSConsEnvironment.WengoCCFlagsAddMultiple = ccflags.WengoCCFlagsAddMultiple
wengoenv.WengoSConsEnvironment.WengoCCFlagsRemove = ccflags.WengoCCFlagsRemove
wengoenv.WengoSConsEnvironment.WengoCCFlagsRemoveMultiple = ccflags.WengoCCFlagsRemoveMultiple

# LinkFlags

wengoenv.WengoSConsEnvironment.WengoLinkFlagsAdd = linkflags.WengoLinkFlagsAdd
wengoenv.WengoSConsEnvironment.WengoLinkFlagsAddMultiple = linkflags.WengoLinkFlagsAddMultiple
wengoenv.WengoSConsEnvironment.WengoLinkFlagsRemove = linkflags.WengoLinkFlagsRemove
wengoenv.WengoSConsEnvironment.WengoLinkFlagsRemoveMultiple = linkflags.WengoLinkFlagsRemoveMultiple

try:
    wengo_api = SCons.Script._SConscript.GlobalDict
except AttributeError:    
    wengo_api =  SCons.Script.SConscript.GlobalDict

wengo_api["WengoEnvironment"] = wengoenv.WengoGetCachedEnvironment
wengo_api["WengoQtEnvironment"] = qtenv.WengoSConsQt3Environment
wengo_api["WengoQt3Environment"] = qtenv.WengoSConsQt3Environment
wengo_api["WengoQt4Environment"] = qtenv.WengoSConsQt4Environment
wengo_api["WengoArguments"] = wengoenv.WengoGetArguments
