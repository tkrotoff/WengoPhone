from wengoscons import wengoenv
import os

#env = wengoenv.getGlobalEnvironment(tools = ['nsis', 'mingw'], toolpath = ['wengoscons/Tools'])
env = wengoenv.getGlobalEnvironment(tools = ['nsis', 'default'], toolpath = ['wengoscons/Tools'])

if env.File('SOptions').exists():
	env.SConscript('SOptions')

#Duplicate = 0 is very important: it tells SCons
#to not duplicate the source code inside the build directory;
#without this it cannot work.

if os.environ.has_key('CC'):
	env.Replace(CC = os.environ['CC'])
if os.environ.has_key('CXX'):
	env.Replace(CXX = os.environ['CXX'])

env.SConscript('SConscript', build_dir = env.WengoGetRootBuildDir(), duplicate = 0)
