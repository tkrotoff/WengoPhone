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

import SCons.Defaults
from SCons.Script import SConscript
from SCons.Script.SConscript import SConsEnvironment
import os, time, sys, re, types, shutil, stat

try:
	ARGUMENTS = SCons.Script.ARGUMENTS
except AttributeError:
	from SCons.Script.SConscript import Arguments
	ARGUMENTS = Arguments

#FIXME Is this a good idea to put this here?
_libs = {}

class WengoSConsEnvironment(SConsEnvironment):
	"""
	WengoSCons main class.

	Inherits from SConsEnvironment and adds very high level functions
	in order to simplify SConscript files.

	All methods and functions starting with Wengo* can be used in SConscript files.
	All other methods and functions should not be used and are internal.

	Functions starting with Wengo* work with the global WengoSConsEnvironment.
	Methods starting with env.Wengo* work with the local WengoSConsEnvironment.


	Generic SConscript template:

	<pre>
	env = WengoGetEnvironment()

	libs = [
		'project1',
		'project2'
	]
	lib_path = [
		'project1/lib',
		'project2/lib'
	]
	include_path = [
		'project1/include',
		'project2/include'
	]
	defines = {
		'DEFINE1' : 1,
		'DEFINE2' : 10
	}
	cc_flags = []
	link_flags = []
	headers = [
		'myheader.h'
	]
	sources = [
		'mysource.cpp'
	]

	if WengoOSWindows():
		libs += [
			'libwindows1',
			'libwindows2'
		]
		defines['WIN32'] = 1

	if WengoCCMSVC():
		cc_flags += ['-myflag']

	if WengoOSLinux():
		libs += ['liblinux1']
		defines['LINUX'] = 1
		sources += ['mylinuxsource.cpp']

	env.WengoAddLibPath(lib_path)
	env.WengoAddCCFlags(cc_flags)
	env.WengoAddLinkFlags(link_flags)
	env.WengoAddIncludePath(include_path)
	env.WengoAddDefines(defines)
	env.WengoUseLibraries(libs)

	env.WengoStaticLibrary('myproject', sources, headers)
	#env.WengoSharedLibrary('myproject', sources, headers)
	#env.WengoProgram('myproject', sources, headers)
	</pre>

	If you want to understand how works WengoSCons, check:
	- WengoProgram()
	- WengoSharedLibrary()
	- WengoStaticLibrary()
	- WengoUseLibraries()
	- __declareLibrary()
	- __useLibraries()
	- __useChildLibraries()
	- __getAllDependencies()
	This methods are WengoSCons heart.

	OS and compiler detection follows the file global.h

	@author David Ferlier
	@author Laurent Almeras
	@author Tanguy Krotoff
	@author Philippe Bernery
	"""

	class CCFlags:
		"""
		Class that deals with compiler flags.

		Compiler flags (CCFLAGS) are used for each file being compiled.
		"""

		def addCCFlags(env, flags):
			"""
			@see WengoSConsEnvironment.WengoAddCCFlags()
			"""

			for flag in flags:
				if not flag in env['CCFLAGS']:
					env.Append(CCFLAGS = flag)
		addCCFlags = staticmethod(addCCFlags)

		def removeCCFlags(env, flags):
			"""
			Removes compiler flags from the Environment.

			@type flags stringlist
			@param flags compiler flags to remove
			"""

			for flag in flags:
				if flag in env['CCFLAGS']:
					env['CCFLAGS'].remove(flag)
		removeCCFlags = staticmethod(removeCCFlags)

		def getCCFlags(env):
			"""
			Gets the CCFLAGS associated with the Environment.

			@rtype stringlist
			@return CCFLAGS (compiler flags)
			"""

			return env['CCFLAGS']
		getCCFlags = staticmethod(getCCFlags)


	class LinkFlags:
		"""
		Class that deals with link flags.

		Link flags (LINKFLAGS) are used only during the link process.
		"""

		def addLinkFlags(env, flags):
			"""
			@see WengoSConsEnvironment.WengoAddLinkFlags()
			"""

			for flag in flags:
				#FIXME This does not work under MacOSX
				#because of -framework QtGui blah blah...
				#if not flag in env['LINKFLAGS']:
					env.Append(LINKFLAGS = flag)
		addLinkFlags = staticmethod(addLinkFlags)

		def removeLinkFlags(env, flags):
			"""
			Removes link flags from the Environment.

			@type flags stringlist
			@param flags link flags to remove
			"""

			for flag in flags:
				if flag in env['LINKFLAGS']:
					env['LINKFLAGS'].remove(flag)
		removeLinkFlags = staticmethod(removeLinkFlags)

		def getLinkFlags(env):
			"""
			Gets the LINKFLAGS associated with the Environment.

			@rtype stringlist
			@return LINKFLAGS
			"""

			return env['LINKFLAGS']
		getLinkFlags = staticmethod(getLinkFlags)


	class LibPath:
		"""
		Class that deals with the library path.

		The linker searches for libraries inside the library path.
		"""

		def addLibPath(env, paths):
			"""
			@see WengoSConsEnvironment.WengoAddLibPath()
			"""

			for path in paths:
				if not path in env['LIBPATH']:
					env.Append(LIBPATH = path)
		addLibPath = staticmethod(addLibPath)

		def getLibPath(env):
			"""
			Gets the LIBPATH associated with the Environment.

			@rtype stringlist
			@return LIBPATH
			"""

			return env['LIBPATH']
		getLibPath = staticmethod(getLibPath)


	class Libs:
		"""
		Class that deals with the linked libraries.

		Libraries the linker will use to link the final library or application.
		"""

		def addLibs(env, libs):
			"""
			Adds libs to the LIBS SCons variable.

			@type libs stringlist
			@param libs libs to add
			"""

			for lib in libs:
				#Fix for MinGW because MinGW has some problems with libraries order
				#during linking (MSVC does not have this issue)
				#This line modifies the libraries order
				if lib in env['LIBS']:
					env['LIBS'].remove(lib)

				env.Append(LIBS = lib)
		addLibs = staticmethod(addLibs)

		def getLibs(env):
			"""
			Gets the LIBS associated with the Environment.

			@rtype stringlist
			@return LIBS
			"""

			return env['LIBS']
		getLibs = staticmethod(getLibs)


	class CC:
		"""
		Detects the compiler used.

		Currently supported compilers are GCC and Microsoft Visual C++.
		"""

		def isGCC(env):
			if 'gcc' in WengoSConsEnvironment.CC.__getTools(env):
				return True
			return False
		isGCC = staticmethod(isGCC)

		def isMinGW(env):
			if 'mingw' in WengoSConsEnvironment.CC.__getTools(env):
				return True
			return False
		isMinGW = staticmethod(isMinGW)

		def isMSVC(env):
			if 'msvc' in WengoSConsEnvironment.CC.__getTools(env):
				return True
			return False
		isMSVC = staticmethod(isMSVC)

		def __getTools(env):
			return env['TOOLS']
		__getTools = staticmethod(__getTools)


	class OS:
		"""
		Detects the operating system used.

		Currently working OS are Windows, Linux and MacOSX.
		"""

		def isWindows():
			return WengoSConsEnvironment.OS.__platformMatch('win32')
		isWindows = staticmethod(isWindows)

		def isWindowsCE():
			#Maybe has to be seen as a compiler like inside global.h
			if WengoSConsEnvironment.OS.isWindows():
				version = sys.getwindowsversion()
				if version == sys.VER_PLATFORM_WIN32_CE:
					return True
			return False
		isWindowsCE = staticmethod(isWindowsCE)

		def isLinux():
			return WengoSConsEnvironment.OS.__platformMatch('linux')
		isLinux = staticmethod(isLinux)

		def isMacOSX():
			return WengoSConsEnvironment.OS.__platformMatch('darwin')
		isMacOSX = staticmethod(isMacOSX)

		def isBSD():
			return WengoSConsEnvironment.OS.__platformMatch('bsd')
		isBSD = staticmethod(isBSD)

		def __getOS():
			return sys.platform
		__getOS = staticmethod(__getOS)

		def __platformMatch(platform):
			if re.search(platform, WengoSConsEnvironment.OS.__getOS()):
				return True
			return False
		__platformMatch = staticmethod(__platformMatch)


	class Defines:
		"""
		Class that deals with the compiler defines.

		Defines will be used for each compiled file.
		"""

		def addDefines(env, defines):
			if len(defines) != 0:
				for define, value in defines.iteritems():
					env.Append(CPPDEFINES = {define : value})
		addDefines = staticmethod(addDefines)

		def exportDefines(env, defines):
			if len(defines) != 0:
				for define, value in defines.iteritems():
					env.Append(CPPDEFINES = {define : value})
		exportDefines = staticmethod(exportDefines)

		def getDefines(env):
			"""Returns the CPPDEFINES associated with the Environment"""
			return env['CPPDEFINES']
		getDefines = staticmethod(getDefines)

		def getExportedDefines(env):
			return env['CPPDEFINES']
		getExportedDefines = staticmethod(getExportedDefines)


	class IncludePath:
		"""
		Class that deals with the include paths.

		The compiler will search for header files inside the include paths.
		"""

		def addIncludePath(env, paths):
			for path in paths:
				if not path in env['CPPPATH']:
					env.Append(CPPPATH = path)
		addIncludePath = staticmethod(addIncludePath)

		def getIncludePath(env):
			"""Returns the CPPPATH associated with the Environment"""
			return env['CPPPATH']
		getIncludePath = staticmethod(getIncludePath)


	class CCGCC:
		"""
		GCC compiler support.
		"""

		def __init__(self):
			cflags = []
			ldflags = []
			if os.environ.has_key('CFLAGS'):
				cflags = os.environ['CFLAGS'].split(' ')
			self.__CCFlags = cflags
			if os.environ.has_key('LDFLAGS'):
				ldflags = os.environ['LDFLAGS'].split(' ')
			self.__linkFlags = ldflags

		def setDebugMode(self):
			self.__setDefaultFlags()
			self.__CCFlags += ['-g']

		def setReleaseMode(self):
			self.__setDefaultFlags()

		def setReleaseModeWithSymbols(self):
			self.setReleaseMode()

		def __setDefaultFlags(self):
			#-fno-common is needed under MacOSX
			self.__CCFlags += ['-fno-common']
			self.__linkFlags += ['-fno-common']

		def getCCWarningFlags(self):
			return ['-Wall']
			#FIXME More GCC warnings
			#return ['-W -Wall -ansi -std=c++98 -pedantic \
			#	-Weffc++ -Wold-style-cast -Wshadow -Winline -Wcast-qual -Werror']

		def getCCFlags(self):
			return self.__CCFlags

		def getLinkFlags(self):
			return self.__linkFlags


	class CCMSVC:
		"""
		MSVC (Microsoft Visual C++) compiler support.
		"""

		def __init__(self):
			self.__CCFlags = []
			self.__linkFlags = []

		def setDebugMode(self):
			self.__setDefaultFlags()
			self.__CCFlags += ['-Zi', '/LDd', '-Gm', '/DEBUG', '-D_DEBUG', '/MDd']
			self.__linkFlags += ['/DEBUG', '/NODEFAULTLIB:MSVCRT.LIB']

		def setReleaseMode(self):
			self.__setDefaultFlags()
			self.__CCFlags += ['/O1', '/MD']
			self.__linkFlags += ['/NODEFAULTLIB:MSVCRTD.LIB']

		def setReleaseModeWithSymbols(self):
			self.__setDefaultFlags()
			self.__CCFlags += ['/O1', '/MD', '/Zi']
			self.__linkFlags += ['/DEBUG', '/NODEFAULTLIB:MSVCRTD.LIB']

		def __setDefaultFlags(self):
			self.__CCFlags = ['/Zm200', '/EHsc', '/GX', '/GR']
			self.__linkFlags = []

		def getCCWarningFlags(self):
			return ['/W3', '-w34100', '-w34189', '/wd4290']

		def getCCFlags(self):
			return self.__CCFlags

		def getLinkFlags(self):
			return self.__linkFlags

	def __init__(self, *args, **kwargs):
		additionalParams = {}
		additionalParams['CPPPATH'] = []
		additionalParams['CPPDEFINES'] = {}
		additionalParams['LIBPATH'] = []
		additionalParams['LIBS'] = []
		additionalParams['ENV'] = os.environ
		kwargs.update(additionalParams)
		SConsEnvironment.__init__(self, *args, **kwargs)

		#Stores SCons file signatures
		self.SConsignFile()

		#Caches implicit dependencies
		self.SetOption('implicit_cache', 1)

		#Do not fetch files from SCCS or RCS subdirectories
		#Desactivated: avoid extra searches and speed up the build a little
		self.SourceCode('.', None)

		#Sets the current compiler
		self.currentCC = None
		if self.CC.isMSVC(self):
			self.currentCC = self.CCMSVC()
			#FIXME This is done for compatibility reason
			self.WengoAddDefines({'WIN32' : 1})
		elif self.CC.isGCC(self):
			self.currentCC = self.CCGCC()
		elif self.CC.isMinGW(self):
			self.currentCC = self.CCGCC()
		else:
			self.currentCC = self.CCGCC()

		#Build mode ('debug', 'release' or 'release-symbols')
		self.__buildMode = None

		#Parses the command line parameters
		self.__parseConsoleArguments()

		#Project name
		self.__projectName = None

		#Cannot do that here, this does not work
		#self.__saveCurrentBuildPath()
		#self.__saveCurrentSourcePath()

		#see WengoSetVariable() and WengoGetVariable()
		self.__variables = {}

		#Command line arguments
		self.__consoleArguments = {}
		for arg, value in ARGUMENTS.iteritems():
			self.__consoleArguments[arg] = value

		#Aliases, see Scons.Alias()
		#self.__aliases = {}

		#By default warnings are activated
		self.__activeWarnings()

		#System library (e.g Qt, boost...) or internal library?
		#By default internal library
		self.__isSystemLibrary = False

		#MacOS X app template path
		self.__macOSXTemplatePath = None

	def isReleaseMode(self):
		"""
		Checks if the compilation is done in release mode.

		@rtype boolean
		@return true if compilation done in release mode; false otherwise
		"""

		return self.__compilationModeMatch('release')

	def isReleaseModeWithSymbols(self):
		"""
		Checks if the compilation is done in release mode with debug symbols.

		Release mode with debug symbols is only available for Microsoft Visual C++:
		debug symbols are included even if the general compilation is done in release mode.
		This is a modification at the application link.

		@rtype boolean
		@return true if compilation done in release mode with symbols; false otherwise
		"""

		return self.__compilationModeMatch('release-symbols')

	def isDebugMode(self):
		"""
		Checks if the compilation is done in debug mode.

		@rtype boolean
		@return true if compilation done in debug mode; false otherwise
		"""

		return self.__compilationModeMatch('debug')

	def __getMode(self):
		"""
		Gets the string representation of the compilation mode.

		@rtype string
		@return 'release' or 'debug' or 'release-symbols'
		"""

		return self.__buildMode

	def __compilationModeMatch(self, mode):
		"""
		Helper function that check the compilation mode.

		@type mode string
		@param mode compilation mode to check ('release' or 'debug' or 'release-symbols')
		@rtype boolean
		@return true if compilation mode checked is the compilation used; false otherwise
		"""

		if self.__getMode() == mode:
			return True
		return False

	def __activeWarnings(self):
		"""
		Activates the compiler warnings.
		"""

		self.WengoAddCCFlags(self.currentCC.getCCWarningFlags())

	def setReleaseMode(self):
		"""
		Sets the compilation mode to release.

		Private, but cannot be named with __ since it is used by WengoSetReleaseMode()
		"""

		self.__buildMode = 'release'

		self.currentCC.setReleaseMode()
		self.WengoAddCCFlags(self.currentCC.getCCFlags())
		self.WengoAddLinkFlags(self.currentCC.getLinkFlags())

	def setReleaseModeWithSymbols(self):
		"""
		Sets the compilation mode to release with debug symbols.

		Private, but cannot be named with __ since it is used by WengoSetReleaseModeWithSymbols()
		"""

		self.__buildMode = 'release-symbols'

		self.currentCC.setReleaseModeWithSymbols()
		self.WengoAddCCFlags(self.currentCC.getCCFlags())
		self.WengoAddLinkFlags(self.currentCC.getLinkFlags())

	def setDebugMode(self):
		"""
		Sets the compilation mode to debug.

		Private, but cannot be named with __ since it is used by WengoSetDebugMode()
		"""

		self.__buildMode = 'debug'

		self.currentCC.setDebugMode()
		#self.WengoAddDefines({'DEBUG' : 1})
		self.WengoAddCCFlags(self.currentCC.getCCFlags())
		self.WengoAddLinkFlags(self.currentCC.getLinkFlags())

	def __setProjectName(self, name):
		"""
		Sets the project name (e.g 'phapi', 'osip', 'wengophone').

		@type name string
		@param name project name
		"""

		self.__projectName = name
		print self.__getProjectName(), '(internal)'

	def __getProjectName(self):
		"""
		Gets the project name.

		@rtype string
		@return project name
		"""

		return self.__projectName

	def WengoProgram(self, programName, sources, headers = []):
		"""
		Generates a program e.g a .exe file given its name and source code.

		All dependencies are handled properly like if the program were a kind of static library.
		if program A depends on static library B that needs C, there is no need inside A's SConscript
		to specify the need of C: B's SConscript already says that there is a dependency with C.
		If B becomes a shared library there is no need to modify A's SConscript which is nice.
		This permits to encapsulate/modularize dependencies.

		Can generate a .vcproj file for Microsoft Visual C++.

		@type programName string
		@param programName name of the program
		@type sources stringlist
		@param sources list of source file (e.g .cpp/.c)
		@rtype SCons project
		@return SCons project generated by SCons
		"""

		tmp = programName
		if self.OS.isMacOSX():
			#Generates a .app under MacOS X otherwise it creates a console application
			tmp += '.app'
			if (self.__macOSXTemplatePath):
				self.__prepareMacOSXApp(self.__macOSXTemplatePath, tmp)
			tmp += '/Contents/MacOS/' + programName

		self.__setProjectName(programName)
		prog = self.Program(tmp, sources)
		self.__createVCProj(prog, sources, headers)
		self.__installProject(prog)
		self.__useChildLibraries()
		self.__installPDBFile()
		return prog

	def WengoCopyToBuildDir(self, src, dest = ''):
		"""
		Copies a directory 'src' to the root build directory.

		The copy (files + subdirectories) is recursive.

		@type src string
		@param src source directory to copy
		@type dest string
		@param dest destination directory where to copy src
		@see __synchronizeDirectories()
		"""

		self.__saveCurrentBuildPath()
		self.__saveCurrentSourcePath()

		src2 = os.path.abspath(os.path.join(self.__getSourcePath(), src))
		dest2 = self.__getBuildDirAbsPath(dest)
		self.__synchronizeDirectories(src2, dest2)

	def __synchronizeDirectories(self, src, dest):
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

		print 'Copying files from', src,

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
					print '.',

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
					print '.',
		print ''

	def WengoAutoconf(self, libraryName, outputFiles,
		configureCommand = './configure', makeCommand = 'make',
		configureFiles = 'configure', makeFiles = 'Makefile'):
		"""
		Build a library using autotools: configure and make.

		Algorithm:
		- Copy the library source directory to the build path directory
		- Launch the configure command
		- Launch the make command
		- Declare the library as an internal library

		@type libraryName string
		@param libraryName name of the library to compile
		@type outputFiles stringlist
		@param outputFiles list of awaited files (.a, .so, .dll, .dylib...)
		@type configureCommand string
		@param configureCommand command line for 'configure' (e.g './configure --enable-gtk --disable-blahblah')
		@type makeCommand string
		@param makeCommand command line for 'make' (e.g 'make blahblah')
		@type configureFiles string
		@param configureFiles files needed by configure (e.g the configure script 'configure' is needed by configure)
		@type makeFiles string
		@param makeFiles files needed by male (e.g the file 'Makefile' is needed by make)
		"""

		self.__saveCurrentBuildPath()
		self.__saveCurrentSourcePath()

		srcDir = self.__getSourcePath()
		buildDir = self.__getBuildPath()

		self.__synchronizeDirectories(srcDir, buildDir)

		makefile = self.Command(makeFiles, configureFiles,
					'cd ' + buildDir + ' && ' + configureCommand)
		make = self.Command(outputFiles, makeFiles,
					'cd ' + buildDir + ' && ' + makeCommand)

		self.Depends(make, makefile)
		self.Alias(libraryName, [make])
		self.WengoDeclareLibrary(libraryName, outputFiles)
		for file in outputFiles:
			#Gets the file path
			file = os.path.split(file)[0]
			self.WengoAddLibPath([file])

	def WengoDeclareMacOSXAppTemplate(self, templatePath):
		"""
		Creates a .app for a program under MacOS X.

		@type templatePath string
		@param templatePath path to the template .app directory
		@see WengoProgram()
		@see __prepareMacOSXApp()
		"""

		self.__macOSXTemplatePath = templatePath

	def __prepareMacOSXApp(self, templatePath, appPath):
		"""
		Prepares the .app folder for MacOS X application.

		Copy resources files in a .app template folder in the real .app folder.

		Example:
		templatePath = 'macosx/template.app'
		appPath = 'qtwengophone-ng.app'

		Content of SOURCE_DIR/macosx/template.app will be copied in BUILD_DIR/qtwengophone.app

		@type templatePath string
		@param templatePath path to the .app template directory
		@type appPath string
		@param appPath path to the .app of the application
		@see WengoProgram()
		@see WengoDeclareMacOSXAppTemplate()
		"""

		self.__saveCurrentBuildPath()
		self.__saveCurrentSourcePath()

		src = os.path.abspath(os.path.join(self.__getSourcePath(), templatePath))
		dest = os.path.join(self.__getBuildPath(), appPath)

		if not os.path.exists(dest):
			os.makedirs(dest)
		print 'Preparing MacOS X application '
		self.__synchronizeDirectories(src, dest)
		print 'done'

	def WengoSharedLibrary(self, libraryName, sources, headers = []):
		"""
		Generates a shared library e.g a .dll/.so file given its name and source code.

		Dependencies changes between shared and static libraries under Windows
		are automatically handled: there is no need to heavily modify the SConscript file
		if you switch from a static library to a shared one.

		Can generate a .vcproj file for Microsoft Visual C++.

		@type libraryName string
		@param libraryName name of the library
		@type sources stringlist
		@param sources list of source file (e.g .cpp/.c)
		@rtype SCons project
		@return SCons project generated using SCons
		"""

		self.__setProjectName(libraryName)
		lib = self.SharedLibrary(libraryName, sources)
		self.__createVCProj(lib, sources, headers)
		self.__installProject(lib)
		self.__useChildLibraries()
		self.__installPDBFile()
		return lib

	def WengoPlugin(self, pluginName, sources, headers = []):
		"""
		Generates a plugin.

		A plugin is a shared library without a prefix ('lib' under UNIX).

		TODO add a pluginVersion parameter

		@see WengoSharedLibrary()
		"""

		lib = None
		self.__setProjectName(pluginName)
		try:
			#SCons 0.96.91 support (pre-release for 0.97)
			tmp = self['LDMODULEPREFIX']
			#Removes the library name prefix
			self['LDMODULEPREFIX'] = ''
			lib = self.LoadableModule(pluginName, sources)
			self['LDMODULEPREFIX'] = tmp
		except KeyError:
			#SCons 0.96.1 support (stable version)
			tmp = self['SHLIBPREFIX']
			#Removes the library name prefix
			self['SHLIBPREFIX'] = ''
			lib = self.SharedLibrary(pluginName, sources)
			self['SHLIBPREFIX'] = tmp

		self.__createVCProj(lib, sources, headers)
		self.__installProject(lib)
		self.__useChildLibraries()
		self.__installPDBFile()
		return lib

	def __useChildLibraries(self):
		"""
		Links (uses) with the child libraries of the current Environment.

		This is used under Windows in order to deal with the change of
		dependencies between shared and static libraries.
		"""

		libs = self.__getAllDependencies([self.__getProjectName()])
		libs.remove(self.__getProjectName())
		for name in libs:
			libEnv = _libs.get(name, None)
			if libEnv:
				self.Alias(self.__getProjectName(), libEnv.__getProjectName())
				self.__linkWithLibrary(libEnv)
			else:
				self.__addLibs([name])

	def WengoStaticLibrary(self, libraryName, sources, headers = []):
		"""
		Generates a static library e.g a .lib/.a file given its name and source code.

		Can generate a .vcproj file for Microsoft Visual C++.

		@type libraryName string
		@param libraryName name of the library
		@type sources stringlist
		@param sources list of source file (e.g .cpp/.c)
		@rtype SCons project
		@return SCons project generated using SCons
		"""

		self.__setProjectName(libraryName)
		lib = self.StaticLibrary(libraryName, sources)
		self.__createVCProj(lib, sources, headers)
		self.__installProject(lib)
		return lib

	def __installProject(self, project):
		"""
		Installs the project generated file (.dll, .lib, .exe) of the current Environment to the root build directory.

		@type project SCons project
		@param project SCons project generated using SCons functions Program() or StaticLibrary() or SharedLibrary()
		"""

		self.__declareLibrary(self.__getProjectName())
		self.Alias(self.__getProjectName(), project)

		install = self.Install(self.WengoGetRootBuildDir(), project)
		self.Alias(self.__getProjectName(), install)

	def __installPDBFile(self):
		"""
		Installs the .pdb file of the current Environment to the root build directory.

		Only available if in debug mode + release mode with debug symbols under Microsoft Visual C++.
		"""

		if self.CC.isMSVC(self) and (self.isDebugMode() or self.isReleaseModeWithSymbols()):
			install = self.Install(self.WengoGetRootBuildDir(), self.__getProjectName() + '.pdb')
			self.Alias(self.__getProjectName(), install)

	def __createVCProj(self, project, sources, headers):
		"""
		Generates a project file for Microsoft Visual C++ (.vcproj).

		@type project SCons project
		@param project SCons project generated using SCons
		@type sources stringlist
		@param sources sources files (e.g .c, .cpp)
		@type headers stringlist
		@param headers headers files (e.g .h)
		"""

		if self.getConsoleArgument('build-vcproj') and self.CC.isMSVC(self):
			srcs = []
			for src in sources:
				if isinstance(src, types.StringTypes):
					srcs.append(src)

			incs = []
			for inc in headers:
				if isinstance(inc, types.StringTypes):
					incs.append(inc)

			vcproj = self.MSVSProject(
				target = self.__getProjectName() + self['MSVSPROJECTSUFFIX'],
				srcs = srcs,
				incs = incs,
				buildtarget = project,
				variant = 'Debug')

			self.Depends(project, vcproj)

	def __getAllDependencies(self, libs):
		"""
		Gets the complete list of the child libraries of the current Environment.

		This method is recursive.

		@rtype stringlist
		@return all the child libraries of the current Environment
		"""

		list = []
		for name in libs:
			env = _libs.get(name, None)
			if not env:
				list += [name]
			else:
				childLibs = env.Libs.getLibs(env)
				if len(childLibs) == 0:
					list += [name]
				else:
					list += [name] + self.__getAllDependencies(childLibs)
		return list

	def setVariable(self, variable, value):
		"""
		Sets a variable with a value than can be used later on.

		Private, but cannot be named with __ since it is used by WengoSetVariable().

		@type variable string
		@param variable variable name
		@type value string
		@param value variable value
		"""

		self.__variables[variable] = value

	def getVariable(self, variable):
		"""
		Gets a variable value given its name.

		Private, but cannot be named with __ since it is used by WengoGetVariable().

		@type variable string
		@param variable variable name
		@rtype string
		@return variable value
		"""

		return self.__variables.get(variable, None)

	def __parseConsoleArguments(self):
		"""
		Parses the arguments/options given through the command line.

		FIXME mode release always becomes mode release-symbols
		"""

		#By default we are in debug mode
		self.__buildMode = ARGUMENTS.get('mode', 'debug')

		#FIXME mode release always becomes mode release-symbols
		if self.__buildMode == 'release':
			self.__buildMode = 'release-symbols'
			print 'Warning: release mode switched to release-symbols mode'

		if self.isDebugMode():
			self.setDebugMode()
		elif self.isReleaseMode():
			self.setReleaseMode()
		elif self.isReleaseModeWithSymbols():
			self.setReleaseModeWithSymbols()
		else:
			print 'Error: incorrect build mode, back to debug mode'
			self.setDebugMode()

	def addConsoleArguments(self, args):
		"""
		Adds an argument to the SCons system like it was set as a command line (console) argument.

		Private, but cannot be named with __ since it is used by WengoAddConsoleArguments().

		@type args dictonnary
		@param args console arguments to add to the SCons system
		"""

		for arg, value in args.iteritems():
			self.__consoleArguments[arg] = ARGUMENTS.get(arg, value)

	def getConsoleArgument(self, arg):
		"""
		Checks the presence of an argument on the command line (console).

		Private, but cannot be named with __ since it is used by WengoGetConsoleArgument().

		@type arg string
		@param arg console argument to check
		@rtype string or boolean
		@return true/false if the argument is activated/desactivated or the string option of the argument
		"""

		tmp = self.__consoleArguments.get(arg, None)
		if tmp == '1' or tmp == 'yes' or tmp == 'YES' or tmp == 'true' or tmp == 'TRUE':
			return True
		elif tmp == '0' or tmp == 'no' or tmp == 'NO' or tmp == 'false' or tmp == 'FALSE':
			return False
		return tmp

	def WengoAddCCFlags(self, flags):
		"""
		Adds flags to the CCFLAGS SCons variable.

		@type flags stringlist
		@param flags list of flags to add
		"""

		self.CCFlags.addCCFlags(self, flags)

	def WengoAddLinkFlags(self, flags):
		"""
		Adds flags to the LINKFLAGS SCons variable.

		@type flags stringlist
		@param flags list of flags to add
		"""

		self.LinkFlags.addLinkFlags(self, flags)

	def WengoAddLibPath(self, paths):
		"""
		Adds paths to the LIBPATH SCons variable.

		@type paths stringlist
		@param paths paths to add
		"""

		self.LibPath.addLibPath(self, paths)

	def __addLibs(self, libs):
		"""
		@see WengoSConsEnvironment.Libs.addLibs()
		"""

		self.Libs.addLibs(self, libs)

	def WengoAddDefines(self, defines):
		"""
		Adds defines to the CPPDEFINES SCons variable.

		@type defines dictonnary
		@param defines defines to add
		"""

		self.Defines.addDefines(self, defines)

	def WengoAddIncludePath(self, paths):
		"""
		Adds paths to the CPPPATH SCons variable.

		@type paths stringlist
		@param paths paths to add
		"""

		self.IncludePath.addIncludePath(self, paths)

	def __declareLibrary(self, projectName):
		"""
		Declares an internal library and add it to the dictionnary of libraries.

		@type projectName string
		@param projectName library project name to register
		"""

		if not self.__getProjectName():
			self.__setProjectName(projectName)
		self.__saveCurrentBuildPath()
		_libs[projectName] = self

	def WengoDeclareLibrary(self, projectName, files):
		"""
		Declares an internal library and add it to the dictionnary of libraries.

		This is usefull for pre-compiled libraries.

		@type projectName string
		@param projectName library project name to register
		@type files stringlist
		@param files list of files (.lib/.dll/.a...) to install
		"""

		#self.__isSystemLibrary = True
		self.__declareLibrary(projectName)
		for file in files:
			install = self.Install(self.WengoGetRootBuildDir(), file)
			self.Alias(self.__getProjectName(), install)

	def WengoDeclareSystemLibrary(self, projectName):
		"""
		Declares a system library and add it to the dictionnary of libraries.

		@see __declareLibrary()
		"""

		self.__isSystemLibrary = True
		self.__declareLibrary(projectName)

	def WengoGetRootBuildDir(self):
		"""
		Gets the root build directory.

		Example: 'C:/build/debug/'

		@rtype string
		@return the root build directory
		"""

		return os.path.abspath(os.path.join(self['ROOT_BUILD_DIR'], self.__getMode()))

	def __getBuildDirAbsPath(self, subpath):
		"""
		Concats a subpath to the current Environment root build directory.

		Example:
		subpath = 'toto'
		result = 'C:/build/debug/toto/'

		@rtype string
		@return current Environment build directory + subpath
		"""

		return os.path.abspath(os.path.join(self.WengoGetRootBuildDir(), subpath))

	def __saveCurrentBuildPath(self):
		"""
		Saves the current build path.
		"""

		self['BUILD_PATH'] = self.Dir('.').path

	def __getBuildPath(self):
		"""
		Gets the build path.

		Example:
		'C:/build/debug/project/subproject/

		@rtype string
		@return Environment build path
		@see __saveCurrentBuildPath()
		"""

		return os.path.abspath(os.path.join(self['ROOT_BUILD_DIR'], self['BUILD_PATH']))

	def __saveCurrentSourcePath(self):
		"""
		Saves the current source code path.
		"""

		self['SOURCE_PATH'] = self.Dir('.').srcnode().path

	def __getSourcePath(self):
		"""
		Gets the source code path.

		Example:
		'C:/build/project/subproject/src'

		@rtype string
		@return Environment source path
		@see __saveCurrentSourcePath()
		"""

		return os.path.abspath(os.path.join(self['ROOT_BUILD_DIR'], self['SOURCE_PATH']))

	def __linkWithLibrary(self, libEnv):
		"""
		Links with a library given its Environment.

		@type libEnv Environment
		@param libEnv library Environment to link with
		"""

		if libEnv.__isSystemLibrary:
			self.WengoAddLibPath(libEnv.LibPath.getLibPath(libEnv))
			self.__addLibs(libEnv.Libs.getLibs(libEnv))
		else:
			self.WengoAddLibPath([self.__getBuildDirAbsPath(libEnv.__getBuildPath())])
			self.__addLibs([libEnv.__getProjectName()])

	def __useLibrary(self, libEnv):
		"""
		Uses (e.g link, include_path, link_flags...) a library given its Environment.

		@type libEnv Environment
		@param libEnv library Environment to use
		"""

		if libEnv.__isSystemLibrary:
			self.WengoAddIncludePath(libEnv.IncludePath.getIncludePath(libEnv))
			self.WengoAddLibPath(libEnv.LibPath.getLibPath(libEnv))
			self.__addLibs(libEnv.Libs.getLibs(libEnv))
			self.WengoAddCCFlags(libEnv.CCFlags.getCCFlags(libEnv))
			self.WengoAddLinkFlags(libEnv.LinkFlags.getLinkFlags(libEnv))
			self.WengoAddDefines(libEnv.Defines.getDefines(libEnv))
		else:
			tmp1 = []
			includePaths = libEnv.IncludePath.getIncludePath(libEnv)
			for includePath in includePaths:
				dir = os.path.join(libEnv.__getBuildPath(), includePath)
				tmp1.append(self.__getBuildDirAbsPath(dir))
			self.WengoAddIncludePath(tmp1)

			#LibPath for libEnv (e.g debug/libs/curl if the project is curl)
			tmp2 = [self.__getBuildDirAbsPath(libEnv.__getBuildPath())]
			self.WengoAddLibPath(tmp2)

			#LibPaths from libEnv that were added inside libEnv using WengoAddLibPath()
			tmp3 = []
			libPaths = libEnv.LibPath.getLibPath(libEnv)
			for libPath in libPaths:
				dir = os.path.join(libEnv.__getBuildPath(), libPath)
				tmp3.append(self.__getBuildDirAbsPath(dir))
			self.WengoAddLibPath(tmp3)

			self.__addLibs([libEnv.__getProjectName()])

			#FIXME Not needed to use the same defines
			#Sometimes defines are used inside headers
			#self.WengoAddDefines(libEnv.Defines.getExportedDefines(libEnv))

	def WengoUseLibraries(self, libs):
		"""
		Uses (e.g link, include_path, link_flags...) a list of libraries given their names.

		@type libs stringlist
		@param libs libraries to use
		"""

		for name in libs:
			libEnv = _libs.get(name, None)
			if libEnv:
				self.__useLibrary(libEnv)
			else:
				print name, '(system)'
				self.__addLibs([name])

	def WengoAddDoxyfile(self):
		"""
		Adds a Doxyfile for documentation generation using doxygen.

		Doxygen documentation is generated using the 'doxygen' command argument.
		The configuration file for doxygen cannot be specified so that all projects have
		the same name for it: 'Doxyfile'.

		@see http://doxygen.org
		"""

		#Saves the current path because doxygen
		#is not running in the correct directory
		self.__saveCurrentSourcePath()
		doc = self.Command(os.path.join(self.__getSourcePath(), 'doc'), 'Doxyfile', 'cd $SOURCE_PATH & doxygen')
		#FIXME duplicate alias cf warning message from SCons
		self.Alias('doxygen', doc)

	def WengoCompileWindowsResource(self, rcFile):
		"""
		Compiles the Windows resource file .rc.

		@type rcFile string
		@param rcFile Windows resource file .rc
		@rtype SCons node
		@return Windows resource file compiled (.res)
		"""

		#FIXME This does not work with MinGW since the resource program of MinGW
		#has to be launched in the same directory where the .rc file is
		#Visual C++ does not have this limitation
		if self.OS.isWindows():
			return self.RES(rcFile)
		return None 

        def WengoQtEdition(self):
            """
            Get edition name of Qt setup.
            """

            import re
            matched = False
            try:
                if WengoOSPosix():
                    qconfig_file = open(os.path.join(os.environ['QTDIR'], 'include', 'QtCore', 'qconfig.h'))
                else :
                    qconfig_file = open(os.path.join(os.environ['QTDIR'], 'src', 'corelib', 'global', 'qconfig.h'))
            except os.IOError:
                pass
            regexp = re.compile('^#\s*define\s+qt_edition\s+.*(\w+)', re.IGNORECASE)
            for line in qconfig_file.readlines():
                match = regexp.match(line)
                if match:
                    matched = True
                    print 'Found Qt Edition : %s' % match.group(1)
                    break
            if matched:
                return match.group(1)
            else:
                raise 'QtEditionNotFound'
            
	def WengoCompileQt4Resource(self, qrcFile):
		"""
		Compiles the Qt4 resource file .qrc.

		@type qrcFile string
		@param qrcFile Qt4 resource file .qrc
		@rtype string
		@return Qt4 resource file compiled (.cpp)
		"""

		qtdir = os.environ['QTDIR']
		rcc = os.path.join(qtdir, 'bin', 'rcc')

		self.__saveCurrentSourcePath()
		os.chdir(self.__getSourcePath())

		outputCppFile = qrcFile + '.cpp'

		os.system(rcc + ' ' + qrcFile + ' -o ' + outputCppFile)
		return outputCppFile

	def WengoCreateFile(self, filename, fileTemplate, fileData):
		"""
		Creates a file inside the source path directory given a template and datas.

		Example:
		<pre>
		WengoPhoneBuildIdTemplate = ""
		#include "WengoPhoneBuildId.h"
		const unsigned long long WengoPhoneBuildId::BUILDID = %(buildId)uULL;
		const char * WengoPhoneBuildId::VERSION = "%(version)s";
		const unsigned long long WengoPhoneBuildId::REVISION = %(revision)uULL;
		const char * WengoPhoneBuildId::SOFTPHONE_NAME = "%(sofphoneName)s";
		""

		WengoPhoneBuildId = {
			'buildId': WengoGetCurrentDateTime(),
			'version': '2.0',
			'revision': WengoGetSubversionRevision(),
			'sofphoneName': 'wengo'
		}

		env.WengoCreateFile('WengoPhoneBuildId.cpp', WengoPhoneBuildIdTemplate, WengoPhoneBuildId),
		</pre>

		@type filename string
		@param filename file to create
		@type fileTemplate string
		@param fileTemplate template of the file to create
		@type fileData string
		@param fileData datas of the file to create
		@rtype string
		@return file created
		"""

		self.__saveCurrentSourcePath()
		os.chdir(self.__getSourcePath())
		fd = open(filename, 'w')
		fd.write(fileTemplate % fileData)
		fd.close()
		return filename


#FIXME ugly?
WengoSConsEnvironment._globalEnv = None

def getGlobalEnvironment(*args, **kwargs):
	"""
	Gets the unique general/global Environment.

	This is a singleton.

	@rtype Environment
	@return global Environment
	"""

	#FIXME This is quite ugly isn't?

	#Lazy initialization
	if not WengoSConsEnvironment._globalEnv:
		#Singleton WengoSConsEnvironment
		WengoSConsEnvironment._globalEnv = WengoSConsEnvironment(*args, **kwargs)

		#Saves the root build directory path
		WengoSConsEnvironment._globalEnv['ROOT_BUILD_DIR'] = WengoSConsEnvironment._globalEnv.Dir('.').abspath

	return WengoSConsEnvironment._globalEnv

def WengoGetEnvironment():
	"""
	Gets a local Environment.

	A local Environment is a copy of the global Environment.

	@rtype Environment
	@return local Environment
	"""

	return getGlobalEnvironment().Copy()

def WengoGetQt3Environment():
	"""
	Gets a local Qt3 Environment.

	@rtype Environment
	@return Qt3 Environment
	"""

	return getGlobalEnvironment().Copy(tools = ['qt'], LIBS = [])

def WengoGetQt4Environment():
	"""
	Gets a local Qt4 Environment.

	@rtype Environment
	@return Qt4 Environment
	"""

	return WengoGetQt3Environment()

def WengoGetRootBuildDir():
	"""
	@see WengoSConsEnvironment.WengoGetRootBuildDir()
	"""

	env = getGlobalEnvironment()
	return env.WengoGetRootBuildDir()

def WengoAddCCFlags(flags):
	"""
	Adds CCFlags to the global Environment.

	@see WengoSConsEnvironment.WengoAddCCFlags()
	"""

	env = getGlobalEnvironment()
	env.WengoAddCCFlags(flags)

def WengoAddDefines(defines):
	"""
	Adds defines to the global Environment.

	@see WengoSConsEnvironment.WengoAddDefines()
	"""

	env = environments.getGlobalEnvironment()
	env.WengoAddDefines(defines)

def WengoAddIncludePath(paths):
	"""
	Adds include paths to the global Environment.

	@see WengoSConsEnvironment.WengoAddIncludePath()
	"""

	env = environments.getGlobalEnvironment()
	env.WengoAddIncludePath(paths)

def WengoCCMinGW():
	"""
	@see WengoSConsEnvironment.CC.isMinGW()
	"""

	env = getGlobalEnvironment()
	return env.CC.isMinGW(env)

def WengoCCGCC():
	"""
	@see WengoSConsEnvironment.CC.isGCC()
	MinGW is GCC
	"""

	if WengoCCMinGW():
		return True

	env = getGlobalEnvironment()
	return env.CC.isGCC(env)

def WengoCCMSVC():
	"""
	@see WengoSConsEnvironment.CC.isMSVC()
	"""

	env = getGlobalEnvironment()
	return env.CC.isMSVC(env)

def WengoOSWindows():
	"""
	@see WengoSConsEnvironment.OS.isWindows()
	"""

	env = getGlobalEnvironment()
	return env.OS.isWindows()

def WengoOSWindowsCE():
	"""
	@see WengoSConsEnvironment.OS.isWindowsCE()
	"""

	env = getGlobalEnvironment()
	return env.OS.isWindowsCE()

def WengoOSLinux():
	"""
	@see WengoSConsEnvironment.OS.isLinux()
	"""

	env = getGlobalEnvironment()
	return env.OS.isLinux()

def WengoOSMacOSX():
	"""
	@see WengoSConsEnvironment.OS.isMacOSX()
	"""

	env = getGlobalEnvironment()
	return env.OS.isMacOSX()

def WengoOSBSD():
	"""
	@see WengoSConsEnvironment.OS.isBSD()
	"""

	env = getGlobalEnvironment()
	return env.OS.isBSD()

def WengoOSPosix():
	"""
	Check if the OS follows the Posix standard.

	@rtype boolean
	@return true if the OS is Posix compliant; false otherwise
	"""

	if WengoOSLinux() or WengoOSMacOSX() or WengoOSBSD():
		return True
	else:
		return False

def WengoArchX86():
	"""
	Checks if the architecture is compatible with X86.

	@rtype boolean
	@return true if the architecture used is compatible with x86, false otherwise
	"""

	import platform
	return platform.machine() == "i386"

def WengoArchPPCMacintosh():
	"""
	Checks if the architecture is compatible with PPC as used on Macintosh computers.

	@rtype boolean
	@return true if the architecture used is compatible with PPC as used on Macintosh computers, false otherwise
	"""

	import platform
	return platform.machine() == "Power Macintosh"

def WengoSetDebugMode():
	"""
	@see WengoSConsEnvironment.setDebugMode()
	"""

	env = getGlobalEnvironment()
	env.setDebugMode()

def WengoDebugMode():
	"""
	@see WengoSConsEnvironment.isDebugMode()
	"""

	env = getGlobalEnvironment()
	return env.isDebugMode()

def WengoSetReleaseMode():
	"""
	@see WengoSConsEnvironment.isDebugMode()
	"""

	env = getGlobalEnvironment()
	env.setReleaseMode()

def WengoReleaseMode():
	"""
	@see WengoSConsEnvironment.isReleaseMode()
	"""

	env = getGlobalEnvironment()
	return env.isReleaseMode()

def WengoSetReleaseModeWithSymbols():
	"""
	@see WengoSConsEnvironment.setReleaseModeWithSymbols()
	"""

	env = getGlobalEnvironment()
	env.setReleaseModeWithSymbols()

def WengoReleaseSymbolsMode():
	"""
	@see WengoSConsEnvironment.isReleaseModeWithSymbols()
	"""
	
	env = getGlobalEnvironment()
	return env.isReleaseModeWithSymbols()

def WengoSetVariable(variable, value):
	"""
	@see WengoSConsEnvironment.setVariable()
	"""

	env = getGlobalEnvironment()
	env.setVariable(variable, value)

def WengoGetVariable(variable):
	"""
	@see WengoSConsEnvironment.getVariable()
	"""

	env = getGlobalEnvironment()
	return env.getVariable(variable)

def WengoAddConsoleArguments(args):
	"""
	@see WengoSConsEnvironment.addConsoleArguments()
	"""

	env = getGlobalEnvironment()
	env.addConsoleArguments(args)

def WengoGetConsoleArgument(arg):
	"""
	@see WengoSConsEnvironment.getConsoleArgument()
	"""

	env = getGlobalEnvironment()
	return env.getConsoleArgument(arg)

def WengoShowWindowsConsole(show):
	"""
	Shows or not the Windows console.

	@type show boolean
	@param show true if the Windows console should be showed; false otherwise
	"""

	env = getGlobalEnvironment()

	if WengoCCMSVC():
		if show:
			env.LinkFlags.removeLinkFlags(env, ['/subsystem:windows'])
			env.WengoAddLinkFlags(['/subsystem:console'])
		else:
			env.LinkFlags.removeLinkFlags(env, ['/subsystem:console'])
			env.WengoAddLinkFlags(['/subsystem:windows'])

def WengoUseLibraries(libs):
	"""
	Adds libraries to the global Environment.

	@see WengoSConsEnvironment.WengoUseLibraries()
	"""

	env = getGlobalEnvironment()
	env.WengoUseLibraries(libs)

def WengoGetSubversionRevision():
	"""
	Gets the Subversion revision number using the shell command 'svn info'

	You need the official Subversion command-line client installed and referenced inside your PATH.
	Under Windows TortoiseSVN is not enough.

	Example of a 'svn info' output:
	Path: .
	URL: http://tanguy_k@dev.openwengo.com/svn/openwengo/trunk
	Repository UUID: 30a43799-04e7-0310-8b2b-ea0d24f86d0e
	Revision: 3448
	Node Kind: directory
	Schedule: normal
	Last Changed Author: tanguy_k
	Last Changed Rev: 3447
	Last Changed Date: 2005-12-09 20:24:12 +0100 (Fri, 09 Dec 2005)

	@rtype integer
	@return Subversion revision number or 0 if an error occured
	"""

	cmd = 'svn info ' + '\"' + getGlobalEnvironment()['ROOT_BUILD_DIR'] + '\"'
	info = os.popen(cmd)
	for line in info.readlines():
		line = line.split(':')
		if len(line) > 1:
			revLine = line[0].strip().lower()
			if revLine[2:] == 'vision':
				return eval(line[1].strip())
	return 0

def WengoGetCurrentDateTime():
	"""
	Gets the current date-time using the format YYYYMMDDHHMMSS.

	Example: 20051210125200 (meaning 2005-12-10 12:52:00)

	@rtype integer
	@return current date-time
	"""

	return eval(time.strftime('%Y%m%d%H%M%S', time.gmtime()))
