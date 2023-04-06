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

def WengoCCFlagsAdd(env, flag):
    env.Append(CCFLAGS = flag)

def WengoCCFlagsAddMultiple(env, flags):
    for flag in flags:
        WengoCCFlagsAdd(env, flag)

def WengoCCFlagsRemove(env, flag):
    if flag in env["CCFLAGS"]:
        env["CCFLAGS"].remove(flag)

def WengoCCFlagsRemoveMultiple(env, cflags):
    for cflag in cflags:
        WengoCCFlagsRemove(env, cflag)
