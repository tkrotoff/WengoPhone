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
import popen2

def __language_release(target, source, env):
    lrelease_path = os.path.join(env["QTDIR"], 'bin', 'lrelease')
    for a_target, a_source in zip(target, source):
        out, infd = popen2.popen4(lrelease_path + " " + str(a_source) + " -qm " + str(a_target))
	print "wengophone-lrelease-target: Updated " + str(a_target)

def WengoQtLanguageRelease(env, source, target):
    return env.Command(target, source, __language_release)

wengoenv.WengoEnvInstallFunction("WengoQtLanguageRelease",
    WengoQtLanguageRelease)
