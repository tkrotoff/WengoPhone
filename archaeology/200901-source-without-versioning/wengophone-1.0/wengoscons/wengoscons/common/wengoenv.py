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

import SCons.Defaults
from SCons.Script import SConscript
from SCons.Script.SConscript import SConsEnvironment
import os, re, platform, sys, struct, shutil

try:
    ARGUMENTS = SCons.Script.ARGUMENTS
except AttributeError:
    from SCons.Script.SConscript import Arguments
    ARGUMENTS = Arguments
    
_cached_env = {}

def WengoGetCachedEnvironment(name = "default"):
    if not name in _cached_env.keys():
        _cached_env[name] = WengoSConsEnvironment()
    return _cached_env[name]

def WengoGetArguments():
    env = WengoGetCachedEnvironment()
    return env.WengoGetConsoleArguments()

class WengoSConsEnvironment(SConsEnvironment):
    def __init__(self, *args, **kwargs):
        self.__cachedValues = {}
        self.__buildDirRoot = None
        self.__buildMode = None
        self.__mode_cc_flags = None
        self.__mode_link_flags = None
        self.__warning_cflags = []
        self.__warning_cxxflags = []
        self.__consoleArguments = {}
        self.__aliases = {}
        self.__root_build_dir = None

        additional_params = {}
        additional_params["CPPPATH"] = []
        additional_params["CPPDEFINES"] = {}
        additional_params["LIBPATH"] = []
        additional_params["LIBS"] = []
        additional_params["ENV"] = os.environ
        kwargs.update(additional_params)
        SConsEnvironment.__init__(self, *args, **kwargs)

        self.__calculateBuildDir()
        self.__calculateWarningFlags()
        self.__parseArguments()
        self.SConsignFile()
        self.SetOption("implicit_cache", 1)
        self.SourceCode(".", None)

        self.__initWin32Specific()

    def __initWin32Specific(self):
        if self.WengoOsIsWindows():
            self.WengoCppDefineAdd("WIN32")

    def __calculateWarningFlags(self):
        tools = self.WengoGetTools()
        if "msvc" in tools:
            self.__warning_cflags = [ '/W3' ]
            self.__warning_cxxflags = [ '/W3', '-w34100', '-w34189' ]
        elif "gcc" in tools:
            self.__warning_cflags = [ '-Wall' ]
            self.__warning_cxxflags = self.__warning_cflags

    def __parseArguments(self):
        mode = ARGUMENTS.get('mode', 'debug')
        self.WengoSetCompilationMode(mode)
        warnings = ARGUMENTS.get('activate-warnings', 'yes')
        if warnings == "yes":
            self.WengoActivateWarnings()
        elif warnings == "no":
            self.WengoDesactivateWarnings()

    def __calculateBuildDir(self):
        if os.environ.has_key("WENGO_BUILD_DIR"):
            self.__buildDirRoot = os.environ["WENGO_BUILD_DIR"]
        else:
            self.__buildDirRoot = self.Dir("build-wengo").abspath

    def WengoActivateWarnings(self):
        self.WengoCCFlagsAddMultiple(self.__warning_cflags)

    def WengoDesactivateWarnings(self):
        self.WengoCCFlagsRemoveMultiple(self.__warning_cflags)

    def WengoGetCompilationMode(self):
        return self.__buildMode

    def WengoSetCompilationMode(self, mode):
        if mode not in [ "debug", "release", "release-symbols" ]:
            return -1

        if self.__mode_cc_flags:
            self.WengoCCFlagsRemoveMultiple(self.__mode_cc_flags)
        if self.__mode_link_flags:
            self.WengoLinkFlagsRemoveMultiple(self.__mode_link_flags)

        self.WengoCppDefineAdd("DEBUG")

        tools = self.WengoGetTools()
        if "msvc" in tools:
            self.__WengoSetCompilationModeMsvc(mode)
        else:
            self.__WengoSetCompilationModeGcc(mode)

        self.__buildMode = mode
        self.__root_build_dir = self.Dir(self.__buildDirRoot + os.sep + \
                    self.WengoGetOs() + "-" + self.__buildMode).abspath

    def __WengoSetCompilationModeGcc(self, mode):
        if mode == "debug":
            self.__mode_cc_flags = ["-g"]
            self.__mode_link_flags = []
        else:
            self.__mode_cc_flags = [""]
            self.__mode_link_flags = []

        # Add -fPIC for 64bit
        if self.WengoMachineIs64bit():
            self.__mode_cc_flags += [ "-fPIC" ]

        if os.environ.has_key('CFLAGS'):
            self.__mode_cc_flags += os.environ['CFLAGS'].split(' ')
        if os.environ.has_key('LDFLAGS'):
            self.__mode_link_flags += os.environ['LDFLAGS'].split(' ')

        self.WengoCCFlagsAddMultiple(self.__mode_cc_flags)
        self.WengoLinkFlagsAddMultiple(self.__mode_link_flags)

    def __WengoSetCompilationModeMsvc(self, mode):
        self.__mode_cc_flags = ['/Zm200', '/EHsc', '/GX', '/GR']

        if mode == "debug":
            self.__mode_cc_flags += [ '-Zi', '/LDd', '-Gm', '/DEBUG', '-D_DEBUG', '/MDd']
            self.__mode_link_flags = [ '/debug' ]
        elif mode == "release":
            self.__mode_cc_flags += ['/O1', '/MD']
            self.__mode_link_flags = []
        elif mode == "release-symbols":
            self.__mode_cc_flags += ['/O1', '/MD', '/Zi']
            self.__mode_link_flags = ['/DEBUG']

        self.WengoCCFlagsAddMultiple(self.__mode_cc_flags)
        self.WengoLinkFlagsAddMultiple(self.__mode_link_flags)

    def WengoCacheEnvironment(self, name = "default"):
        _cached_env[name] = self

    def WengoGetTools(self):
        return self["TOOLS"]

    def WengoGetArguments(self):
        return ARGUMENTS

    def WengoGetWindowsVersion(self):
        version = sys.getwindowsversion()
        if version == sys.VER_PLATFORM_WIN32s:
            return "windows-3.1"
        elif version == sys.VER_PLATFORM_WIN32_WINDOWS:
            return "win98"
        elif version == sys.VER_PLATFORM_WIN32_NT:
            return "winnt"
        elif version == sys.VER_PLATFORM_WIN32_CE:
            return "wince"

    def WengoGetRootBuildDir(self):
        return self.__root_build_dir

    def WengoGetRootBuildDirRelative(self):
        return "build-wengo" + os.sep + self.WengoGetOs() + "-" + self.__buildMode

    def WengoGetBuildDirRelativePath(self, subpath):
        build_dir = self.WengoGetRootBuildDir()
        return self.Dir(os.path.join(build_dir, subpath)).abspath

    def WengoSConscript(self, script, builddir):
        self.SConscript(script,
            build_dir = self.WengoGetRootBuildDir() + os.sep + builddir,
            duplicate = 1)

    def WengoBuildInDir(self, script, builddir):
        return self.SConscript(script,
            build_dir = self.WengoGetRootBuildDir() + os.sep + builddir,
            duplicate = 1)

    def WengoProgram(self, *args, **kwargs):
        prog = self.Program(*args, **kwargs)
        if self.WengoGetConsoleArgument("build-vcproj") == "yes":
            vcproj = self.WengoCreateVCProj(args[0], args[1], prog)
            self.Depends(prog, vcproj)        

        if self.WengoOsMatch("freebsd"):
            self.WengoLinkFlagsAdd("-pthread")

        if self.WengoOsIsWindows() and 0:
            if self.WengoGetCompilationMode() == "debug":
                self.WengoLinkFlagsAdd("/subsystem:console")
            else:
                self.WengoLinkFlagsAdd("/subsystem:windows")
        return prog


    def WengoSharedLibrary(self, *args, **kwargs):
        lib = self.SharedLibrary(*args, **kwargs)
        if self.WengoOsMatch("freebsd"):
            self.WengoLinkFlagsAdd("-pthread")
        if self.WengoGetConsoleArgument("build-vcproj") == "yes":
                vcproj = self.WengoCreateVCProj(args[0], args[1], lib)
                self.Depends(lib, vcproj)
        return lib

    def WengoStaticLibrary(self, *args, **kwargs):
        lib = self.StaticLibrary(*args, **kwargs)
        if self.WengoGetConsoleArgument("build-vcproj") == "yes":
            vcproj = self.WengoCreateVCProj(args[0], args[1], lib)
            self.Depends(lib, vcproj)
        return lib

    def WengoStaticObject(self, *args, **kwargs):
        return self.StaticObject(*args, **kwargs)

    def WengoSharedObject(self, *args, **kwargs):
        return self.SharedObject(*args, **kwargs)

    def WengoSetVariable(self, variable, value):
        self.__cachedValues[variable] = value

    def WengoGetVariable(self, variable):
        return self.__cachedValues.get(variable, None)

    def WengoGetConsoleArgument(self, arg):
        return self.__consoleArguments.get(arg, None)

    def WengoGetConsoleArguments(self):
        return self.__consoleArguments

    def WengoDeclareConsoleArgument(self, arg, default):
        self.__consoleArguments[arg] = ARGUMENTS.get(arg, default)

    def WengoAlias(self, name, tgt):
        self.Alias(name, tgt)
        self.__aliases[name] = tgt

    def WengoGetAlias(self, name):
        return self.__aliases.get(name, None)

    def WengoCopy(self, source, dest):
        return self.Command(dest, source,
                SCons.Defaults.Copy('$TARGET', '$SOURCE'))

    def WengoSyncDirs(self, src, dest):
        return self.Command(dest, src, self._WengoSyncDirs)

    def _WengoSyncDirs(self, target, source, env):
        """
        Synchronizes directory 'dest' with directory 'src'.

        This method copies files and directories if and only if
        the files or the directories in 'dest' do not exist or
        have been changed in 'src'.

        @type src string
        @param src source directory
        @type dest string
        @param dest destination directory
        """

        for dest, src in zip(target, source):
            dest = str(dest)
            src = str(src)
            if not os.path.exists(dest):
                os.makedirs(dest)

            for root, dirs, files in os.walk(src):
                for f in files:
                    absSrcFile = os.path.join(root, f)
                    i = absSrcFile.find(src) + len(src) + 1
                    absDestFile = os.path.join(dest, absSrcFile[i:])
                    if (not os.path.exists(absDestFile)) \
                        or (os.stat(absSrcFile).st_mtime != os.stat(absDestFile).st_mtime):
                        shutil.copy2(absSrcFile, absDestFile)

                #Don't visit CVS, .svn and _svn directories
                repoDirs = ['CVS', '.svn', '_svn']
                for d in repoDirs:
                    if d in dirs:
                        dirs.remove(d)

                for d in dirs:
                    absSrcDir = os.path.join(root, d)
                    i = absSrcDir.find(src) + len(src) + 1
                    absDestDir = os.path.join(dest, absSrcDir[i:])
                    if not os.path.exists(absDestDir):
                        os.mkdir(absDestDir)

def WengoEnvInstallFunction(name, func):
    setattr(WengoSConsEnvironment, name, func)

def WengoInstallClass(ins):
    for key, val in ins.__dict__.iteritems():
        setattr(WengoSConsEnvironment, key, val)

