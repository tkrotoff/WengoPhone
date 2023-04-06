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

def WengoLibPathAdd(env, path):
    env.Append(LIBPATH = path)

def WengoLibPathAddMultiple(env, paths):
    for path in paths:
        WengoLibPathAdd(env, path)

def WengoLibPathRemove(env, path):
    if path in env["LIBPATH"]:
        env["LIBPATH"].remove(path)

def WengoLibAdd(env, library):
    env.Append(LIBS = library)

def WengoLibAddMultiple(env, libraries):
    for library in libraries:
        WengoLibAdd(env, library)

def WengoLibRemove(env, library):
    if library in env["LIBS"]:
        env["LIBS"].remove(library)
