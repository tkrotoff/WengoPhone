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
import os.path

class WengoLibraryHolder:
    def __init__(self, name, **kwargs):
        self.include_paths = []
        self.lib_paths = []
        self.libraries = []
        self.cpp_defines = []
        self.lib_root_path = None
        self.__name = name
        self.__dict__.update(kwargs)

    def AddSubIncludePath(self, spath):
        self.include_paths.append(
            self.env.WengoGetBuildDirRelativePath(
                os.path.join(self.lib_root_path, spath)))

    def AddSubLibPath(self, spath):
        self.lib_paths.append(
            self.env.WengoGetBuildDirRelativePath(
                os.path.join(self.lib_root_path, spath)))

    def RemoveLibrary(self, lib):
        if lib in self.libraries:
            self.libraries.remove(lib)

    def AddLibrary(self, lib):
        self.libraries.append(lib)


    def getName(self):
        return self.__name

    def getIncludePaths(self):
        return self.include_paths

    def getLibraryPaths(self):
        return self.lib_paths
    
    def getLibraries(self):
        return self.libraries

    def getCppDefines(self):
        return self.cpp_defines

def WengoDeclareLibrary(env, name,
            include_paths = [],
            lib_paths = [],
            libraries = []):

    lib = WengoLibraryHolder(name,
        include_paths = include_paths,
        lib_paths = lib_paths, libraries = libraries, env = env)

    env.libdec_libs[name] = lib

def WengoDeclareLibraryAuto(env, name, path):
    include_paths = [ env.WengoGetBuildDirRelativePath(path) ]
    lib_paths = [ env.WengoGetBuildDirRelativePath(path) ]
    libraries = [ name ]

    lib = WengoLibraryHolder(name,
        include_paths = include_paths,
        lib_paths = lib_paths, libraries = libraries, env = env)

    lib.lib_root_path = path

    env.libdec_libs[name] = lib
    return lib

def __findLibrary(env, name):
    return env.libdec_libs.get(name, None)

def WengoUseLibraryHeaders(env, name):
    lib = __findLibrary(env, name)
    if lib:
        env.WengoIncludePathsAddMultiple(lib.getIncludePaths())
    else:
        print "Library", name, "not found !"

def WengoUseLibraryPaths(env, name):
    lib = __findLibrary(env, name)
    if lib:
        env.WengoLibPathAddMultiple(lib.getLibraryPaths())
    else:
        print "Library", name, "not found !"

def WengoUseLibraryLibraries(env, name):
    lib = __findLibrary(env, name)
    if lib:
        env.WengoLibAddMultiple(lib.getLibraries())
    else:
        print "Library", name, "not found !"

def WengoUseLibrary(env, name):
    WengoUseLibraryHeaders(env, name)
    WengoUseLibraryPaths(env, name)
    WengoUseLibraryLibraries(env, name)

wengoenv.WengoSConsEnvironment.libdec_libs = {}
wengoenv.WengoEnvInstallFunction("WengoDeclareLibrary", WengoDeclareLibrary)
wengoenv.WengoEnvInstallFunction("WengoDeclareLibraryAuto", WengoDeclareLibraryAuto)
wengoenv.WengoEnvInstallFunction("WengoUseLibrary", WengoUseLibrary)
wengoenv.WengoEnvInstallFunction("WengoUseLibraryHeaders",
    WengoUseLibraryHeaders)
wengoenv.WengoEnvInstallFunction("WengoUseLibraryLibraries",
    WengoUseLibraryLibraries)
wengoenv.WengoEnvInstallFunction("WengoUseLibraryPaths",
    WengoUseLibraryPaths)
