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

from wengoscons.common import wengoenv
import os
import os.path

def WengoSConsQt3Environment(env):
    """SCons environment for Qt version 3 (include path + library path)"""

    if env.WengoOsIsWindows():
        libs = [ "qui", "qt-mt335" ]
    else:
        libs = [ "qui", "qt-mt" ]

    q_env = env.Copy(tools = [ "qt" ], LIBS = libs)
    q_env.WengoCppDefineAdd("QT_THREAD_SUPPORT")

    includes = []
    includes += [ os.path.join(os.environ['QTDIR'], "include") ]

    inc = q_env.WengoGetVariable("QtIncludePaths")
    if inc:
        includes += [ inc ]

    q_env.WengoIncludePathsAddMultiple(includes)

    return q_env

def WengoSConsQt4Environment(env):
    """SCons environment for Qt version 4 (include path + library path)"""

    if env.WengoGetCompilationMode() == "debug":
        libs = [ "QtCored4", "QtGuid4", "QtNetworkd4", "QtDesignerd4", "QtXmld4", "QAxContainerd" ]
    else:
        libs = [ "QtCore4", "QtGui4", "QtNetwork4", "QtDesigner4", "QtXml4", "QAxContainer" ]

    q_env = env.Copy(tools = [ "qt" ], LIBS = libs)
    q_env.WengoCppDefineAdd("QT_THREAD_SUPPORT")

    qtdir = os.environ['QTDIR']
    includes = []
    includes += [ os.path.join(qtdir, "include") ]
    includes += [ os.path.join(qtdir, "include", "QtCore") ]
    includes += [ os.path.join(qtdir, "include", "QtGui") ]
    includes += [ os.path.join(qtdir, "include", "QtNetwork") ]
    includes += [ os.path.join(qtdir, "include", "QtDesigner") ]
    includes += [ os.path.join(qtdir, "include", "QtXml") ]
    includes += [ os.path.join(qtdir, "include", "ActiveQt") ]

    inc = q_env.WengoGetVariable("QtIncludePaths")
    if inc:
        includes += [ inc ]

    q_env.WengoIncludePathsAddMultiple(includes)

    return q_env

def WengoQtNoScan(env):
    env["QT_AUTOSCAN"] = 0

def WengoMoc(env, *args, **kwargs):
    return env.Moc(*args, **kwargs)

wengoenv.WengoEnvInstallFunction("WengoQtNoScan", WengoQtNoScan)
wengoenv.WengoEnvInstallFunction("WengoMoc", WengoMoc)
