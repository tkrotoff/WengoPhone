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

from wengoscons.common import wengoenv
import os
import os.path

def WengoQt4Resource(env, resource, target):
    """Compiles Qt4 resource file .qrc"""

    qtdir = os.environ['QTDIR']
    env['QT_RCC'] = os.path.join(qtdir, "bin", "rcc")

    print env['QT_RCC']
    return env.Command(target, resource, "$QT_RCC $SOURCES -o $TARGET")

wengoenv.WengoEnvInstallFunction("WengoQt4Resource", WengoQt4Resource)
