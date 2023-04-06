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

_defines = {}

def config_h_build(target, source, env):
    config_h_defines = env.WengoGetVariable("_lastdefines")
    for a_target, a_source in zip(target, source):
        config_h = file(str(a_target), "w")
        config_h_in = file(str(a_source), "r")
        config_h.write(config_h_in.read() % config_h_defines)
        config_h_in.close()
        config_h.close()

def WengoGenerateConfigHeader(env, config_h, config_h_in, config_defines):
    ## No choice for now :S
    env.WengoSetVariable("_lastdefines", config_defines)
    return env.Command(config_h, config_h_in, config_h_build)

wengoenv.WengoEnvInstallFunction("WengoGenerateConfigHeader",
    WengoGenerateConfigHeader)
