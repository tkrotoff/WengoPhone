from wengoscons import wengoenv

#env = wengoenv.getGlobalEnvironment(tools = ['nsis', 'mingw'], toolpath = ['wengoscons/Tools'])
env = wengoenv.getGlobalEnvironment(tools = ['nsis', 'default'], toolpath = ['wengoscons/Tools'])

if env.File('SOptions').exists():
	env.SConscript('SOptions')

#Duplicate = 0 is very important: it tells SCons
#to not duplicate the source code inside the build directory;
#without this it cannot work.
import re
if re.match('3\.\d\.\d', env.WengoCCGCCVersion()):
	env.WengoAddCCFlags(['-pthread'])
	env.WengoAddLinkFlags(['-pthread'])

env.SConscript('SConscript', build_dir = env.WengoGetRootBuildDir(), duplicate = 0)
